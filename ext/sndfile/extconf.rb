require 'rubygems'
require 'mkmf'

$CFLAGS = '-I/opt/local/include'
$LDFLAGS = '-L/opt/local/lib -L/usr/local/lib'

# Add narray header to path
matches = Gem.source_index.find_name 'narray', Gem::Requirement.default
if matches.empty?
  raise 'You need to install NArray gem'
  exit
end
spec = matches.last
$CFLAGS += " -I#{spec.full_gem_path}"

# libsndfile requirements
unless find_library 'sndfile', 'sf_open'
  raise 'You need to install libsndfile (http://www.mega-nerd.com/libsndfile/)'
  exit
end

# NArray gem requirements (double check that header there)
unless have_header 'narray.h'
  raise 'You need to install NArray gem'
  exit
end

# Swig
system 'swig -ruby -I/usr/include -I/usr/local/include -I/opt/local/include sndfile.i'

create_makefile 'sndfile'