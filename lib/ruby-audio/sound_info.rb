module RubyAudio
  class SoundInfo < CSoundInfo
    def initialize options={}
      # Populate from options if given
      unless options.empty?
        options.each {|key,value| send("#{key}=", value)}
      end
    end

    def clone
      SoundInfo.new(:channels => channels, :samplerate => samplerate, :format => format)
    end

    alias_method :seekable?, :seekable
  end
end