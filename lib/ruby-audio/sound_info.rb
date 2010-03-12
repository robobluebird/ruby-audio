module RubyAudio
  # Class <code>SoundInfo</code>  provides information about open sound files'
  # format, length, samplerate, channels, and other things.
  #
  # Example:
  #   snd = RubyAudio::Sound.open("snd.wav")
  #   puts snd.info.channels                  #=> 2
  #   puts snd.info.samplerate                #=> 48000
  #   snd.close
  #
  # In addition it can be used to specify the format of new sound files:
  #
  #   info = RubyAudio::SoundInfo.new :channels => 1, :samplerate => 48000, :format => RubyAudio::FORMAT_WAV|RubyAudio::FORMAT_PCM_16
  #   snd = RubyAudio::Sound.open("new.wav", 'w', info)
  class SoundInfo < CSoundInfo
    # Creates a new SoundInfo object and populates it using the given data
    #
    # Example:
    #    info = RubyAudio::SoundInfo.new :channels => 1, :samplerate => 48000, :format => RubyAudio::FORMAT_WAV|RubyAudio::FORMAT_PCM_16
    def initialize options={}
      # Populate from options if given
      unless options.empty?
        options.each {|key,value| send("#{key}=", value)}
      end
    end

    # Returns a new <code>SoundInfo</code> object that is has the same channel
    # count, sample rate, and format. This is useful in creating a new sound with
    # the same format as an already existing sound.
    #
    # Example:
    #   snd1 = RubyAudio::Sound.open("snd.wav")
    #   snd2 = RubyAudio::Sound.open("snd2.wav", 'w', snd1.info.clone)
    def clone
      SoundInfo.new(:channels => channels, :samplerate => samplerate, :format => format)
    end

    alias_method :seekable?, :seekable
  end
end