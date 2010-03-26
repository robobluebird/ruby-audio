#include <ruby.h>
#include <sndfile.h>

void Init_ra_buffer();
void Init_ra_soundinfo();
void Init_ra_sound();

VALUE eRubyAudioError;

/*
 * Document-module: RubyAudio
 */
/*
 * Document-class: RubyAudio::Error
 *
 * Error class for RubyAudio
 */
void Init_rubyaudio_ext() {
    // Create RubyAudio module and other setup
    VALUE mRubyAudio = rb_define_module("RubyAudio");
    eRubyAudioError = rb_define_class_under(mRubyAudio, "Error", rb_eStandardError);

    // Set up classes
    Init_ra_buffer();
    Init_ra_soundinfo();
    Init_ra_sound();

    // Export libsndfile constants
    // Major formats
    rb_define_const(mRubyAudio, "FORMAT_WAV", INT2FIX(SF_FORMAT_WAV));
    rb_define_const(mRubyAudio, "FORMAT_AIFF", INT2FIX(SF_FORMAT_AIFF));
    rb_define_const(mRubyAudio, "FORMAT_AU", INT2FIX(SF_FORMAT_AU));
    rb_define_const(mRubyAudio, "FORMAT_RAW", INT2FIX(SF_FORMAT_RAW));
    rb_define_const(mRubyAudio, "FORMAT_PAF", INT2FIX(SF_FORMAT_PAF));
    rb_define_const(mRubyAudio, "FORMAT_SVX", INT2FIX(SF_FORMAT_SVX));
    rb_define_const(mRubyAudio, "FORMAT_NIST", INT2FIX(SF_FORMAT_NIST));
    rb_define_const(mRubyAudio, "FORMAT_VOC", INT2FIX(SF_FORMAT_VOC));
    rb_define_const(mRubyAudio, "FORMAT_IRCAM", INT2FIX(SF_FORMAT_IRCAM));
    rb_define_const(mRubyAudio, "FORMAT_W64", INT2FIX(SF_FORMAT_W64));
    rb_define_const(mRubyAudio, "FORMAT_MAT4", INT2FIX(SF_FORMAT_MAT4));
    rb_define_const(mRubyAudio, "FORMAT_MAT5", INT2FIX(SF_FORMAT_MAT5));
    rb_define_const(mRubyAudio, "FORMAT_PVF", INT2FIX(SF_FORMAT_PVF));
    rb_define_const(mRubyAudio, "FORMAT_XI", INT2FIX(SF_FORMAT_XI));
    rb_define_const(mRubyAudio, "FORMAT_HTK", INT2FIX(SF_FORMAT_HTK));
    rb_define_const(mRubyAudio, "FORMAT_SDS", INT2FIX(SF_FORMAT_SDS));
    rb_define_const(mRubyAudio, "FORMAT_AVR", INT2FIX(SF_FORMAT_AVR));
    rb_define_const(mRubyAudio, "FORMAT_WAVEX", INT2FIX(SF_FORMAT_WAVEX));
    rb_define_const(mRubyAudio, "FORMAT_SD2", INT2FIX(SF_FORMAT_SD2));
    rb_define_const(mRubyAudio, "FORMAT_FLAC", INT2FIX(SF_FORMAT_FLAC));
    rb_define_const(mRubyAudio, "FORMAT_CAF", INT2FIX(SF_FORMAT_CAF));
#ifdef HAVE_CONST_SF_FORMAT_OGG
    rb_define_const(mRubyAudio, "FORMAT_OGG", INT2FIX(SF_FORMAT_OGG));
#endif

    // Subtypes from here on
    rb_define_const(mRubyAudio, "FORMAT_PCM_S8", INT2FIX(SF_FORMAT_PCM_S8));
    rb_define_const(mRubyAudio, "FORMAT_PCM_16", INT2FIX(SF_FORMAT_PCM_16));
    rb_define_const(mRubyAudio, "FORMAT_PCM_24", INT2FIX(SF_FORMAT_PCM_24));
    rb_define_const(mRubyAudio, "FORMAT_PCM_32", INT2FIX(SF_FORMAT_PCM_32));
    rb_define_const(mRubyAudio, "FORMAT_PCM_U8", INT2FIX(SF_FORMAT_PCM_U8));
    rb_define_const(mRubyAudio, "FORMAT_FLOAT", INT2FIX(SF_FORMAT_FLOAT));
    rb_define_const(mRubyAudio, "FORMAT_DOUBLE", INT2FIX(SF_FORMAT_DOUBLE));
    rb_define_const(mRubyAudio, "FORMAT_ULAW", INT2FIX(SF_FORMAT_ULAW));
    rb_define_const(mRubyAudio, "FORMAT_ALAW", INT2FIX(SF_FORMAT_ALAW));
    rb_define_const(mRubyAudio, "FORMAT_IMA_ADPCM", INT2FIX(SF_FORMAT_IMA_ADPCM));
    rb_define_const(mRubyAudio, "FORMAT_MS_ADPCM", INT2FIX(SF_FORMAT_MS_ADPCM));
    rb_define_const(mRubyAudio, "FORMAT_GSM610", INT2FIX(SF_FORMAT_GSM610));
    rb_define_const(mRubyAudio, "FORMAT_VOX_ADPCM", INT2FIX(SF_FORMAT_VOX_ADPCM));
    rb_define_const(mRubyAudio, "FORMAT_G721_32", INT2FIX(SF_FORMAT_G721_32));
    rb_define_const(mRubyAudio, "FORMAT_G723_24", INT2FIX(SF_FORMAT_G723_24));
    rb_define_const(mRubyAudio, "FORMAT_G723_40", INT2FIX(SF_FORMAT_G723_40));
    rb_define_const(mRubyAudio, "FORMAT_DWVW_12", INT2FIX(SF_FORMAT_DWVW_12));
    rb_define_const(mRubyAudio, "FORMAT_DWVW_16", INT2FIX(SF_FORMAT_DWVW_16));
    rb_define_const(mRubyAudio, "FORMAT_DWVW_24", INT2FIX(SF_FORMAT_DWVW_24));
    rb_define_const(mRubyAudio, "FORMAT_DWVW_N", INT2FIX(SF_FORMAT_DWVW_N));
    rb_define_const(mRubyAudio, "FORMAT_DPCM_8", INT2FIX(SF_FORMAT_DPCM_8));
    rb_define_const(mRubyAudio, "FORMAT_DPCM_16", INT2FIX(SF_FORMAT_DPCM_16));
#ifdef HAVE_CONST_SF_FORMAT_OGG
    rb_define_const(mRubyAudio, "FORMAT_VORBIS", INT2FIX(SF_FORMAT_VORBIS));
#endif

    // Endian-ness options
    rb_define_const(mRubyAudio, "ENDIAN_FILE", INT2FIX(SF_ENDIAN_FILE));
    rb_define_const(mRubyAudio, "ENDIAN_LITTLE", INT2FIX(SF_ENDIAN_LITTLE));
    rb_define_const(mRubyAudio, "ENDIAN_BIG", INT2FIX(SF_ENDIAN_BIG));
    rb_define_const(mRubyAudio, "ENDIAN_CPU", INT2FIX(SF_ENDIAN_CPU));

    // Format masks
    rb_define_const(mRubyAudio, "FORMAT_SUBMASK", INT2FIX(SF_FORMAT_SUBMASK));
    rb_define_const(mRubyAudio, "FORMAT_TYPEMASK", INT2FIX(SF_FORMAT_TYPEMASK));
    rb_define_const(mRubyAudio, "FORMAT_ENDMASK", INT2FIX(SF_FORMAT_ENDMASK));
}