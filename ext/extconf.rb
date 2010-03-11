require 'mkmf'

$CFLAGS = '-I/opt/local/include'
$LDFLAGS = '-L/opt/local/lib -L/usr/local/lib'

# libsndfile requirements
unless find_library 'sndfile', 'sf_open'
  raise 'You need to install libsndfile (http://www.mega-nerd.com/libsndfile/)'
  exit
end

create_makefile 'rubyaudio_ext'