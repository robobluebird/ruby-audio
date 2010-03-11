# The module that contains everything needed for ruby-audio.
#
# * {RubyAudio::Sound} is the class that represents a sound file, wrapping SNDFILE in libsndfile.
# * {RubyAudio::Buffer} contains the actual sound data.
module RubyAudio
  VERSION = '0.2.1'
end

require 'rubyaudio_ext'
require 'ruby-audio/error'
require 'ruby-audio/buffer'
require 'ruby-audio/sound_info'