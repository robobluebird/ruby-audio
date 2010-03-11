#include "ra_soundinfo.h"

void Init_ra_soundinfo(VALUE mBase) {
    VALUE cRASoundInfo = rb_define_class_under(mBase, "CSoundInfo", rb_cObject);
    rb_define_alloc_func(cRASoundInfo, ra_soundinfo_allocate);
    rb_define_method(cRASoundInfo, "valid?",      ra_soundinfo_valid, 0);
    rb_define_method(cRASoundInfo, "frames",      ra_soundinfo_frames, 0);
    rb_define_method(cRASoundInfo, "samplerate",  ra_soundinfo_samplerate, 0);
    rb_define_method(cRASoundInfo, "samplerate=", ra_soundinfo_samplerate_set, 1);
    rb_define_method(cRASoundInfo, "channels",    ra_soundinfo_channels, 0);
    rb_define_method(cRASoundInfo, "channels=",   ra_soundinfo_channels_set, 1);
    rb_define_method(cRASoundInfo, "format",      ra_soundinfo_format, 0);
    rb_define_method(cRASoundInfo, "format=",     ra_soundinfo_format_set, 1);
    rb_define_method(cRASoundInfo, "sections",    ra_soundinfo_sections, 0);
    rb_define_method(cRASoundInfo, "seekable",    ra_soundinfo_seekable, 0);
}

/*** Initialization and Memory Manangement ***/
static VALUE ra_soundinfo_allocate(VALUE klass) {
    SF_INFO *info = ALLOC(SF_INFO);
    memset(info, 0, sizeof(SF_INFO));
    VALUE self = Data_Wrap_Struct(klass, NULL, ra_soundinfo_free, info);
    return self;
}

static void  ra_soundinfo_free(SF_INFO *info) {
    free(info);
}

/*** Instance Methods ***/
static VALUE ra_soundinfo_valid(VALUE self) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    return sf_format_check(info) ? Qtrue : Qfalse;
}

static VALUE ra_soundinfo_frames(VALUE self) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    return OFFT2NUM(info->frames);
}

static VALUE ra_soundinfo_samplerate(VALUE self) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    return INT2FIX(info->samplerate);
}

static VALUE ra_soundinfo_samplerate_set(VALUE self, VALUE new_samplerate) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    info->samplerate = FIX2INT(new_samplerate);
    return self;
}

static VALUE ra_soundinfo_channels(VALUE self) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    return INT2FIX(info->channels);
}

static VALUE ra_soundinfo_channels_set(VALUE self, VALUE new_channels) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    info->channels = FIX2INT(new_channels);
    return self;
}

static VALUE ra_soundinfo_format(VALUE self) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    return INT2FIX(info->format);
}

static VALUE ra_soundinfo_format_set(VALUE self, VALUE new_format) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    info->format = FIX2INT(new_format);
    return self;
}

static VALUE ra_soundinfo_sections(VALUE self) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    return INT2FIX(info->sections);
}

static VALUE ra_soundinfo_seekable(VALUE self) {
    SF_INFO *info;
    Data_Get_Struct(self, SF_INFO, info);
    return info->seekable ? Qtrue : Qfalse;
}