#include <ruby.h>
#include "ra_buffer.h"

void Init_rubyaudio_ext() {
    VALUE mRubyAudio = rb_const_get(rb_cObject, rb_intern("RubyAudio"));
    Init_ra_buffer(mRubyAudio);
}