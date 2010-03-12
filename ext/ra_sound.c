#include "ra_sound.h"

extern VALUE mRubyAudio, eRubyAudioError;

void Init_ra_sound() {
    VALUE cRASound = rb_define_class_under(mRubyAudio, "CSound", rb_cObject);
    rb_define_alloc_func(cRASound, ra_sound_allocate);
    rb_define_singleton_method(cRASound, "open", ra_sound_s_open, -1);
    rb_define_method(cRASound, "initialize", ra_sound_init, 3);
    rb_define_method(cRASound, "info", ra_sound_info, 0);
    rb_define_method(cRASound, "seek", ra_sound_seek, 2);
    rb_define_method(cRASound, "read", ra_sound_read, 2);
    rb_define_method(cRASound, "write", ra_sound_write, 1);
    rb_define_method(cRASound, "close", ra_sound_close, 0);
    rb_define_method(cRASound, "closed?", ra_sound_closed, 0);
}

/*** Initialization and Memory Manangement ***/
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

/*** Singleton Methods ***/
static VALUE ra_sound_s_open(int argc, VALUE *argv, VALUE klass) {
    VALUE obj = rb_class_new_instance(argc, argv, klass);
    if(!rb_block_given_p()) return obj;
    return rb_ensure(rb_yield, obj, ra_sound_close_safe, obj);
}

/*** Instance Methods ***/
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

static VALUE ra_sound_info(VALUE self) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    return snd->info;
}

static VALUE ra_sound_seek(VALUE self, VALUE frames, VALUE whence) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    if(snd->closed) rb_raise(eRubyAudioError, "closed sound");

    if(sf_seek(snd->snd, (sf_count_t)NUM2OFFT(frames), FIX2INT(whence)) == -1) {
        rb_raise(eRubyAudioError, "invalid seek");
    }

    return INT2FIX(0);
}

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

static VALUE ra_sound_closed(VALUE self) {
    RA_SOUND *snd;
    Data_Get_Struct(self, RA_SOUND, snd);
    return snd->closed ? Qtrue : Qfalse;
}