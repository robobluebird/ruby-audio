module RubyAudio
  class Buffer
    [:short, :int, :float, :double].each do |type|
      eval "def self.#{type}(frames, channels=1); self.new(:#{type}, frames, channels); end"
    end
  end
end