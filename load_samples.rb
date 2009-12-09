require 'ostruct'
require 'pp'
require 'rubygems'
require 'google_chart'

$LOAD_PATH << 'ext/sndfile'
$LOAD_PATH << 'lib'
$LOAD_PATH << 'lib/audio'
require 'audio'
require 'sndfile'
require 'narray'

@files = [
'audio_files/063_1.WAV',
'audio_files/063_2.WAV',
'audio_files/063_3.WAV'
]
  
@sound_files = []
@files.each do |f|
  @sound_files << Audio::Soundfile.open(f)
end

@sounds = []
@sound_files.each do |sf|
  @sounds << sf.read_int(sf.frames)
end

@f = @sounds.first
@l = @sounds.last
