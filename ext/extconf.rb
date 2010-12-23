require 'mkmf'

dir_config('sndfile')

# Mega-Nerd windows installer installs as libsndfile-1.dll
if RUBY_PLATFORM =~ /(mswin|mingw|cygwin)/
  sndfile_lib = 'sndfile-1'
else
  sndfile_lib = 'sndfile'
end

# libsndfile requirements
find_header 'sndfile.h', '/opt/local/include', '/usr/local/include', 'C:/Program Files/Mega-Nerd/libsndfile/include'
unless find_library sndfile_lib, 'sf_open', '/opt/local/lib', '/usr/local/lib', 'C:/Program Files/Mega-Nerd/libsndfile'
  fail <<-EOM
  Can't find libsndfile (http://www.mega-nerd.com/libsndfile/)

  Try passing --with-sndfile-dir or --with-sndfile-lib and --with-sndfile-include
  options to extconf.
  EOM
end

# Check for format support
have_const('SF_FORMAT_OGG', 'sndfile.h')

create_makefile 'rubyaudio_ext'