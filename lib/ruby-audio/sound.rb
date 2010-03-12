module RubyAudio
  class Sound < CSound
    def initialize(path, mode='r', info=nil)
      info ||= SoundInfo.new
      super(path, mode, info)
    end

    def seek(amount, whence=IO::SEEK_SET)
      super(amount, whence)
    end

    def read(*args)
      case args[0]
      when Buffer
        buf = args[0]
        size = args[1] || buf.size
        return super(buf, size)
      when Symbol, String
        type = args[0]
        buf = Buffer.new(type, args[1], info.channels)
        super(buf, buf.size)
        return buf
      else
        raise ArgumentError, "invalid arguments"
      end
    end
  end
end