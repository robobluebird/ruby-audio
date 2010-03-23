#include "ra_buffer.h"

ID ra_short_sym, ra_int_sym, ra_float_sym, ra_double_sym;
extern VALUE eRubyAudioError;

// Before RFLOAT_VALUE, value was in a different place in the struct
#ifndef RFLOAT_VALUE
#define RFLOAT_VALUE(v) (RFLOAT(v)->value)
#endif

/*
 * Class <code>CBuffer</code> is a very light wrapper around a standard C array
 * that can be read from and written to by libsndfile.
 */
void Init_ra_buffer() {
    VALUE mRubyAudio = rb_define_module("RubyAudio");
    VALUE cRABuffer = rb_define_class_under(mRubyAudio, "CBuffer", rb_cObject);
    rb_define_alloc_func(cRABuffer, ra_buffer_allocate);
    rb_define_method(cRABuffer, "initialize",      ra_buffer_init, -1);
    rb_define_method(cRABuffer, "initialize_copy", ra_buffer_init_copy, 1);
    rb_define_method(cRABuffer, "channels",        ra_buffer_channels, 0);
    rb_define_method(cRABuffer, "size",            ra_buffer_size, 0);
    rb_define_method(cRABuffer, "real_size",       ra_buffer_real_size, 0);
    rb_define_method(cRABuffer, "real_size=",      ra_buffer_real_size_set, 1);
    rb_define_method(cRABuffer, "type",            ra_buffer_type, 0);
    rb_define_method(cRABuffer, "[]",              ra_buffer_aref, 1);
    rb_define_method(cRABuffer, "[]=",             ra_buffer_aset, 2);

    ra_short_sym = rb_intern("short");
    ra_int_sym = rb_intern("int");
    ra_float_sym = rb_intern("float");
    ra_double_sym = rb_intern("double");
}

static VALUE ra_buffer_allocate(VALUE klass) {
    RA_BUFFER *buf = ALLOC(RA_BUFFER);
    memset(buf, 0, sizeof(RA_BUFFER));
    VALUE self = Data_Wrap_Struct(klass, NULL, ra_buffer_free, buf);
    return self;
}

static void ra_buffer_free(RA_BUFFER *buf) {
    if(buf->data != NULL) xfree(buf->data);
    xfree(buf);
}

/*
 * Uses size, channels, and type to allocate a properly sized array and set data
 * to the pointer for that data. Returns size.
 */
static int ra_buffer_alloc_data(RA_BUFFER *buf) {
    int size;
    switch(buf->type) {
        case RA_BUFFER_TYPE_SHORT: size = sizeof(short)*buf->size*buf->channels; break;
        case RA_BUFFER_TYPE_INT: size = sizeof(int)*buf->size*buf->channels; break;
        case RA_BUFFER_TYPE_FLOAT: size = sizeof(float)*buf->size*buf->channels; break;
        case RA_BUFFER_TYPE_DOUBLE: size = sizeof(double)*buf->size*buf->channels; break;
    }
    buf->data = (void*)xmalloc(size);
    return size;
}

/*
 * call-seq:
 *   RubyAudio::CBuffer.new(type, size, channels=1) => buf
 *
 * Returns a new <code>CBuffer</code> object which can contain the given number
 * of audio frames of the given data type.
 *
 *   buf = RubyAudio::CBuffer.new("float", 1000)
 */
static VALUE ra_buffer_init(int argc, VALUE *argv, VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);

    // Check args
    if(argc < 2) rb_raise(rb_eArgError, "At least 2 arguments required");

    // Get type of object
    const char *buf_type;
    switch(TYPE(argv[0])) {
        case T_SYMBOL:
            buf_type = rb_id2name(SYM2ID(argv[0]));
            if(!buf_type) rb_raise(rb_eArgError, "bad type");
            break;
        case T_STRING:
            buf_type = RSTRING_PTR(argv[0]);
            break;
        default:
            rb_raise(rb_eArgError, "bad type");
            break;
    }

    // Populate channels
    buf->channels = (argc == 3) ? FIX2INT(argv[2]) : 1;

    // Allocate data array based on type
    buf->size = FIX2INT(argv[1]);
    buf->real_size = 0;
    if(strcmp(buf_type, "short") == 0) buf->type = RA_BUFFER_TYPE_SHORT;
    else if(strcmp(buf_type, "int") == 0) buf->type = RA_BUFFER_TYPE_INT;
    else if(strcmp(buf_type, "float") == 0) buf->type = RA_BUFFER_TYPE_FLOAT;
    else if(strcmp(buf_type, "double") == 0) buf->type = RA_BUFFER_TYPE_DOUBLE;
    else rb_raise(rb_eArgError, "Invalid type: %s", buf_type);
    ra_buffer_alloc_data(buf);

    // Return self
    return self;
}

/* :nodoc: */
static VALUE ra_buffer_init_copy(VALUE copy, VALUE buf) {
    if (copy == buf) return copy;

    // Checks
    rb_check_frozen(copy);
    if (!rb_obj_is_instance_of(buf, rb_obj_class(copy))) {
        rb_raise(rb_eTypeError, "wrong argument class");
    }

    RA_BUFFER *copy_struct, *buf_struct;
    Data_Get_Struct(copy, RA_BUFFER, copy_struct);
    Data_Get_Struct(buf, RA_BUFFER, buf_struct);

    // Clone data
    memcpy(copy_struct, buf_struct, sizeof(RA_BUFFER));
    int size = ra_buffer_alloc_data(copy_struct);
    memcpy(copy_struct->data, buf_struct->data, size);

    return copy;
}

/*
 * call-seq:
 *   buf.channels => integer
 *
 * Returns the number of channels in a frame of the buffer.
 */
static VALUE ra_buffer_channels(VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);
    return INT2FIX(buf->channels);
}

/*
 * call-seq:
 *   buf.size => integer
 *
 * Returns the number of frames the buffer can store.
 */
static VALUE ra_buffer_size(VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);
    return INT2FIX(buf->size);
}

/*
 * call-seq:
 *   buf.real_size => integer
 *
 * Returns the number of frames of actual data are currently stored in the
 * buffer.
 */
static VALUE ra_buffer_real_size(VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);
    return INT2FIX(buf->real_size);
}

/*:nodoc:*/
static VALUE ra_buffer_real_size_set(VALUE self, VALUE real_size) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);

    int new_real_size = FIX2INT(real_size);
    if(new_real_size > buf->size) {
        buf->real_size = buf->size;
    } else if(new_real_size < 0) {
        buf->real_size = 0;
    } else {
        buf->real_size = new_real_size;
    }

    return INT2FIX(buf->real_size);
}

/*
 * call-seq:
 *   buf.type => symbol
 *
 * Returns the type of audio data being stored. <code>:short</code>,
 * <code>:int</code>, <code>:float</code>, or <code>:double</code>.
 */
static VALUE ra_buffer_type(VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);
    switch(buf->type) {
        case RA_BUFFER_TYPE_SHORT: return ID2SYM(ra_short_sym);
        case RA_BUFFER_TYPE_INT: return ID2SYM(ra_int_sym);
        case RA_BUFFER_TYPE_FLOAT: return ID2SYM(ra_float_sym);
        case RA_BUFFER_TYPE_DOUBLE: return ID2SYM(ra_double_sym);
    }
}

/*
 * call-seq:
 *   buf[integer] => frame
 *
 * Returns a frame of audio data at the given offset.
 *
 *   buf = snd.read(:float, 100)     # Mono sound
 *   buf[5]                          #=> 0.4
 *
 *   buf2 = snd2.read(:float, 100)   # Stereo sound
 *   buf[5]                          #=> [0.4, 0.3]
 */
static VALUE ra_buffer_aref(VALUE self, VALUE index) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);

    // Bounds check
    int f = FIX2INT(index);
    if(f < 0 || f >= buf->real_size) return Qnil;
    int i = f * buf->channels;

    if(buf->channels == 1) {
        return ra_buffer_index_get(buf, i);
    } else {
        VALUE frame = rb_ary_new();
        int j;
        for(j = 0; j < buf->channels; j++) {
            rb_ary_push(frame, ra_buffer_index_get(buf, i+j));
        }
        return frame;
    }
}

static VALUE ra_buffer_index_get(RA_BUFFER *buf, int i) {
    switch(buf->type) {
        case RA_BUFFER_TYPE_SHORT: return INT2FIX((int)((short*)buf->data)[i]);
        case RA_BUFFER_TYPE_INT: return INT2FIX(((int*)buf->data)[i]);
        case RA_BUFFER_TYPE_FLOAT: return rb_float_new((double)((float*)buf->data)[i]);
        case RA_BUFFER_TYPE_DOUBLE: return rb_float_new(((double*)buf->data)[i]);
    }
}

/*
 * call-seq:
 *   buf[integer] = numeric => numeric
 *   buf[integer] = array => array
 *
 * Sets the frame of audio data at the given offset to the value. For
 * multi-channel audio, pass in an array of values.
 *
 *   buf = RubyAudio::Buffer.int(100, 1)
 *   buf[0] = 5
 *
 *   buf = RubyAudio::Buffer.double(100, 2)
 *   buf[0] = [0.5, 0.3]
 */
static VALUE ra_buffer_aset(VALUE self, VALUE index, VALUE val) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);

    // Bounds check
    int f = FIX2INT(index);
    if(f < 0 || f >= buf->size) rb_raise(eRubyAudioError, "setting frame out of bounds");
    int i = f * buf->channels;

    // Set data
    if(buf->channels == 1) {
        ra_buffer_index_set(buf, i, val);
    } else {
        if(TYPE(val) != T_ARRAY) rb_raise(eRubyAudioError, "must pass in array for multi-channel buffer");
        long length = RARRAY_LEN(val);
        if(length != buf->channels) rb_raise(eRubyAudioError, "array length must match channel count");

        int j;
        for(j = 0; j < length; j++) {
            ra_buffer_index_set(buf, i+j, rb_ary_entry(val, j));
        }
    }

    // Bump real_size
    if(f + 1 > buf->real_size) {
        buf->real_size = f + 1;
    }

    return val;
}

static void ra_buffer_index_set(RA_BUFFER *buf, int i, VALUE val) {
    switch(buf->type) {
        case RA_BUFFER_TYPE_SHORT:
            ((short*)buf->data)[i] = (short)FIX2INT(val);
            break;
        case RA_BUFFER_TYPE_INT:
            ((int*)buf->data)[i] = FIX2INT(val);
            break;
        case RA_BUFFER_TYPE_FLOAT:
            ((float*)buf->data)[i] = (float)RFLOAT_VALUE(val);
            break;
        case RA_BUFFER_TYPE_DOUBLE:
            ((double*)buf->data)[i] = RFLOAT_VALUE(val);
            break;
    }
}