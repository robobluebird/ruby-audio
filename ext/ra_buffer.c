#include "ra_buffer.h"

ID ra_short_sym, ra_int_sym, ra_float_sym, ra_double_sym;
extern VALUE mRubyAudio, eRubyAudioError;

void Init_ra_buffer() {
    VALUE cRABuffer = rb_define_class_under(mRubyAudio, "CBuffer", rb_cObject);
    rb_define_alloc_func(cRABuffer, ra_buffer_allocate);
    rb_define_method(cRABuffer, "initialize", ra_buffer_init, -1);
    rb_define_method(cRABuffer, "channels",   ra_buffer_channels, 0);
    rb_define_method(cRABuffer, "size",       ra_buffer_size, 0);
    rb_define_method(cRABuffer, "real_size",  ra_buffer_real_size, 0);
    rb_define_method(cRABuffer, "real_size=", ra_buffer_real_size_set, 1);
    rb_define_method(cRABuffer, "type",       ra_buffer_type, 0);
    rb_define_method(cRABuffer, "[]",         ra_buffer_aref, 1);

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
    if(strcmp(buf_type, "short") == 0) {
        buf->type = RA_BUFFER_TYPE_SHORT;
        buf->data = ALLOC_N(short, buf->size * buf->channels);
    } else if(strcmp(buf_type, "int") == 0) {
        buf->type = RA_BUFFER_TYPE_INT;
        buf->data = ALLOC_N(int, buf->size * buf->channels);
    } else if(strcmp(buf_type, "float") == 0) {
        buf->type = RA_BUFFER_TYPE_FLOAT;
        buf->data = ALLOC_N(float, buf->size * buf->channels);
    } else if(strcmp(buf_type, "double") == 0) {
        buf->type = RA_BUFFER_TYPE_DOUBLE;
        buf->data = ALLOC_N(double, buf->size * buf->channels);
    } else {
        rb_raise(rb_eArgError, "Invalid type: %s", buf_type);
    }

    // Return self
    return self;
}

static VALUE ra_buffer_channels(VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);
    return INT2FIX(buf->channels);
}

static VALUE ra_buffer_size(VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);
    return INT2FIX(buf->size);
}

static VALUE ra_buffer_real_size(VALUE self) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);
    return INT2FIX(buf->real_size);
}

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

    return self;
}

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

static VALUE ra_buffer_aref(VALUE self, VALUE index) {
    RA_BUFFER *buf;
    Data_Get_Struct(self, RA_BUFFER, buf);

    // Bounds check
    int i = FIX2INT(index);
    if(i < 0 || i >= buf->real_size) return Qnil;

    if(buf->channels == 1) {
        return ra_buffer_data_index(buf, i);
    } else {
        VALUE frame = rb_ary_new();
        int j;
        for(j = 0; j < buf->channels; j++) {
            rb_ary_push(frame, ra_buffer_data_index(buf, i+j));
        }
        return frame;
    }
}

static VALUE ra_buffer_data_index(RA_BUFFER *buf, int i) {
    switch(buf->type) {
        case RA_BUFFER_TYPE_SHORT: return INT2FIX((int)((short*)buf->data)[i]);
        case RA_BUFFER_TYPE_INT: return INT2FIX(((int*)buf->data)[i]);
        case RA_BUFFER_TYPE_FLOAT: return rb_float_new((double)((float*)buf->data)[i]);
        case RA_BUFFER_TYPE_DOUBLE: return rb_float_new(((double*)buf->data)[i]);
    }
}