#include "ra_sound.h"

extern VALUE eRubyAudioError;

/*
 * Class <code>CSound</code> is a very light wrapper around the
 * <code>SNDFILE</code> struct exposed by libsndfile.
 */
void Init_ra_sound() {
    VALUE mRubyAudio = rb_define_module("RubyAudio");
    VALUE cRASound = rb_define_class_under(mRubyAudio, "CSound", rb_cObject);
    rb_define_alloc_func(cRASound, ra_sound_allocate);
    rb_define_singleton_method(cRASound, "open", ra_sound_s_open, -1);
    rb_define_method(cRASound, "initialize", ra_sound_init, 3);
    rb_define_method(cRASound, "info", ra_sound_info, 0);
    rb_define_method(cRASound, "seek", ra_sound_seek, 2);
    rb_define_method(cRASound, "read", ra_sound_read, 2);
    rb_define_method(cRASound, "write", ra_sound_write, 1);
    rb_define_method(cRASound, "<<", ra_sound_addbuf, 1);
    rb_define_method(cRASound, "close", ra_sound_close, 0);
    rb_define_method(cRASound, "closed?", ra_sound_closed, 0);
}

static VALUE ra_sound_allocate(VALUE klass) {
    RA_SOUND *snd = ALLOC(RA_SOUND);
    memset(snd, 0, sizeof(RA_SOUND));
    VALUE self = Data_Wrap_Struct(klass, ra_sound_mark, ra_sound_free, snd);
    return self;
}

static void ra_sound_mark(RA_SOUND *snd) {
    if(snd) {
        rb_gc_mark(snd->info);
    }
}

static void ra_sound_free(RA_SOUND *snd) {
    if(!snd->closed && snd->snd != NULL) sf_close(snd->snd);
    xfree(snd);
}

/*
 * call-seq:
 *   CSound.open(...)                => snd
 *   CSound.open(...) {|snd| block } => obj
 *
 * With no associated block, <code>open</code> is a synonym for
 * <code>CSound.new</code>. If the optional code block is given, it will be
 * passed <i>snd</i> as an argument, and the CSound object will automatically be
 * closed when the block terminates. In this instance, <code>CSound.open</code>
 * returns the value of the block.
 */
static VALUE ra_sound_s_open(int argc, VALUE *argv, VALUE klass) {
    VALUE obj = rb_class_new_instance(argc, argv, klass);
    if(!rb_block_given_p()) return obj;
    return rb_ensure(rb_yield, obj, ra_sound_close_safe, obj);
}

/*
 * call-seq:
 *   CSound.new(path, mode, info) => snd
 *
 * Returns a new <code>CSound</code> object for the audio file at the given path
 * with the given mode. Valid modes are <code>"r"</code>, <code>"w"</code>, or
 * <code>"rw"</code>.
 */
static VALUE ra_sound_init(VALUE self, VALUE path, VALUE mode, VALUE info) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);

    // Get mode
    const char *m = StringValueCStr(mode);
    if(strcmp(m,      "rw") == 0) snd->mode = SFM_RDWR;
    else if(strcmp(m, "r") == 0)  snd->mode = SFM_READ;
    else if(strcmp(m, "w") == 0)  snd->mode = SFM_WRITE;
    else rb_raise(rb_eArgError, "invalid access mode %s", m);

    // Set info
    snd->info = info;

    // Open sound file
    const char *p = StringValueCStr(path);
    SF_INFO *sf_info;
    Data_Get_Struct(info, SF_INFO, sf_info);
    snd->snd = sf_open(p, snd->mode, sf_info);
    if(snd->snd == NULL) rb_raise(eRubyAudioError, sf_strerror(snd->snd));
    snd->closed = 0;

    return self;
}

/*
 * call-seq:
 *   snd.info => CSoundInfo
 *
 * Returns the info object associated with the sound.
 */
static VALUE ra_sound_info(VALUE self) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    return snd->info;
}

/*
 * call-seq:
 *   snd.seek(frames, whence) => 0
 *
 * Seeks to a given offset <i>anInteger</i> in the sound according to the value
 * of <i>whence</i>:
 *
 *   IO::SEEK_CUR  | Seeks to _frames_ plus current position
 *   --------------+----------------------------------------------------
 *   IO::SEEK_END  | Seeks to _frames_ plus end of stream (you probably
 *                 | want a negative value for _frames_)
 *   --------------+----------------------------------------------------
 *   IO::SEEK_SET  | Seeks to the absolute location given by _frames_
 */
static VALUE ra_sound_seek(VALUE self, VALUE frames, VALUE whence) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    if(snd->closed) rb_raise(eRubyAudioError, "closed sound");

    if(sf_seek(snd->snd, (sf_count_t)NUM2OFFT(frames), FIX2INT(whence)) == -1) {
        rb_raise(eRubyAudioError, "invalid seek");
    }

    return INT2FIX(0);
}

/*
 * call-seq:
 *   snd.read(buf, frames) => integer
 *
 * Tries to read the given number of frames into the buffer and returns the
 * number of frames actually read.
 */
static VALUE ra_sound_read(VALUE self, VALUE buf, VALUE frames) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    if(snd->closed) rb_raise(eRubyAudioError, "closed sound");

    // Get buffer struct
    RA_BUFFER *b;
    Data_Get_Struct(buf, RA_BUFFER, b);

    // Get info struct
    SF_INFO *info;
    Data_Get_Struct(snd->info, SF_INFO, info);

    // Check buffer channels matches actual channels
    if(b->channels != info->channels) {
        rb_raise(eRubyAudioError, "channel count mismatch: %d vs %d", b->channels, info->channels);
    }

    // Double-check frame count against buffer size
    sf_count_t f = (sf_count_t)NUM2OFFT(frames);
    if(f < 0 || f > b->size) {
        rb_raise(eRubyAudioError, "frame count invalid");
    }

    // Read data
    sf_count_t read;
    switch(b->type) {
        case RA_BUFFER_TYPE_SHORT:
            read = sf_readf_short(snd->snd, b->data, f);
            break;
        case RA_BUFFER_TYPE_INT:
            read = sf_readf_int(snd->snd, b->data, f);
            break;
        case RA_BUFFER_TYPE_FLOAT:
            read = sf_readf_float(snd->snd, b->data, f);
            break;
        case RA_BUFFER_TYPE_DOUBLE:
            read = sf_readf_double(snd->snd, b->data, f);
            break;
    }
    b->real_size = read;

    return INT2FIX(b->real_size);
}

/*
 * call-seq:
 *   snd.write(buf) => integer
 *
 * Writes the entire contents of the given buffer to the sound and returns the
 * number of frames written.
 */
static VALUE ra_sound_write(VALUE self, VALUE buf) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    if(snd->closed) rb_raise(eRubyAudioError, "closed sound");

    // Get buffer struct
    RA_BUFFER *b;
    Data_Get_Struct(buf, RA_BUFFER, b);

    // Get info struct
    SF_INFO *info;
    Data_Get_Struct(snd->info, SF_INFO, info);

    // Check buffer channels matches actual channels
    if(b->channels != info->channels) {
        rb_raise(eRubyAudioError, "channel count mismatch: %d vs %d", b->channels, info->channels);
    }

    // Write data
    sf_count_t written;
    switch(b->type) {
        case RA_BUFFER_TYPE_SHORT:
            written = sf_writef_short(snd->snd, b->data, b->real_size);
            break;
        case RA_BUFFER_TYPE_INT:
            written = sf_writef_int(snd->snd, b->data, b->real_size);
            break;
        case RA_BUFFER_TYPE_FLOAT:
            written = sf_writef_float(snd->snd, b->data, b->real_size);
            break;
        case RA_BUFFER_TYPE_DOUBLE:
            written = sf_writef_double(snd->snd, b->data, b->real_size);
            break;
    }

    return OFFT2NUM(written);
}

/*
 * call-seq:
 *   snd << buf => snd
 *
 * Writes the given buffer to the string.
 *
 *   snd << buf1 << buf2
 */
static VALUE ra_sound_addbuf(VALUE self, VALUE buf) {
    ra_sound_write(self, buf);
    return self;
}

/*
 * call-seq:
 *   snd.close => nil
 *
 * Closes <i>snd</i> and frees up all memory associated with the sound. The
 * sound is unavailable for any further data operations; an error is raised if
 * such an attempt is made. Sounds are automatically closed when they are claimed
 * by the garbage collector.
 */
static VALUE ra_sound_close(VALUE self) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    if(snd->closed) rb_raise(eRubyAudioError, "closed sound");

    sf_close(snd->snd);
    snd->snd = NULL;
    snd->closed = 1;
    return Qnil;
}

static VALUE ra_sound_close_safe(VALUE self) {
    return rb_rescue(ra_sound_close, self, 0, 0);
}

/*
 * call-seq:
 *   snd.closed? => true or false
 *
 * Whether or not the current sound is closed to further operations.
 */
static VALUE ra_sound_closed(VALUE self) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    return snd->closed ? Qtrue : Qfalse;
}