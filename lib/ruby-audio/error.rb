module RubyAudio
  # An exception raised by ruby-audio code.
  class Error < StandardError
    # @param message [String] The error message
    def initialize(message = nil)
      super(message)
    end
  end
end