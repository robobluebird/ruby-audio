require 'mkmf'
$CFLAGS = '-I/opt/local/include -I/opt/local/lib/ruby/gems/1.8/gems/narray-0.5.9.7 -I/Library/Ruby/Gems/1.8/gems/narray-0.5.9.7'
$LDFLAGS = '-L/opt/local/lib -L/usr/local/lib'
system 'swig -ruby -I/usr/include -I/usr/local/include -I/opt/local/include sndfile.i'
unless find_library 'sndfile', 'sf_open'
  raise 'You need to install libsndfile'
  exit
end
dir_config 'narray', CONFIG['sitearchdir'], CONFIG['sitearchdir']
# TODO /opt/local/lib/ruby/gems/1.8/gems/narray-0.5.9.4/narray.h
unless have_header 'narray.h'
  raise 'You need to install NArray =(0.5.9.7)'
  exit
end
create_makefile 'sndfile'
