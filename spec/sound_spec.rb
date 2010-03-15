require File.dirname(__FILE__) + '/spec_helper.rb'

describe RubyAudio::Sound do
  MONO_TEST_WAV = File.dirname(__FILE__)+'/data/what.wav'
  STEREO_TEST_WAV = File.dirname(__FILE__)+'/data/what2.wav'
  TEST_MP3 = File.dirname(__FILE__)+'/data/what.mp3'
  OUT_WAV = File.dirname(__FILE__)+'/data/temp.wav'

  after :each do
    File.delete(OUT_WAV) if File.exists?(OUT_WAV)
  end

  it "should open a standard wav without issues" do
    lambda {
      RubyAudio::Sound.open(MONO_TEST_WAV)
    }.should_not raise_error
  end

  it "should raise an exception if the mode is invalid" do
    lambda {
      RubyAudio::Sound.open(MONO_TEST_WAV, 'q')
    }.should raise_error(ArgumentError, 'invalid access mode q')
  end

  it "should close the sound on block exit" do
    s = nil
    RubyAudio::Sound.open(MONO_TEST_WAV) do |snd|
      snd.closed?.should be_false
      s = snd
    end
    s.closed?.should be_true
  end

  it "should raise an exception for an unsupported file" do
    lambda {
      RubyAudio::Sound.open(TEST_MP3)
    }.should raise_error(RubyAudio::Error, "File contains data in an unknown format.")
  end

  it "should raise an exception if file does not exist" do
    lambda {
      RubyAudio::Sound.open(TEST_MP3+'.not')
    }.should raise_error(RubyAudio::Error, "System error : No such file or directory.")
  end

  it "should have the proper sound info" do
    RubyAudio::Sound.open(MONO_TEST_WAV) do |snd|
      snd.info.channels.should == 1
      snd.info.samplerate.should == 16000
      snd.info.format.should == RubyAudio::FORMAT_WAV|RubyAudio::FORMAT_PCM_16
    end
  end

  it "should allow seeking" do
    lambda {
      RubyAudio::Sound.open(MONO_TEST_WAV) do |snd|
        snd.seek(100)
        buf = snd.read(:float, 100)
        buf[0].should > 0
      end
    }.should_not raise_error
  end

  it "should raise exceptions for invalid seeks" do
    lambda {
      RubyAudio::Sound.open(MONO_TEST_WAV) {|snd| snd.seek(-1)}
    }.should raise_error(RubyAudio::Error, "invalid seek")
    lambda {
      RubyAudio::Sound.open(MONO_TEST_WAV) {|snd| snd.seek(1000000)}
    }.should raise_error(RubyAudio::Error, "invalid seek")
  end

  it "should allow reading samples from the sound" do
    RubyAudio::Sound.open(STEREO_TEST_WAV) do |snd|
      buf = snd.read(:float, 1000)
      buf.size.should == 1000
      buf.real_size.should == 1000
      buf[999].length.should == 2
    end
  end

  it "should allow reading into an existing buffer" do
    buf = RubyAudio::Buffer.float(1000)
    buf.real_size.should == 0
    RubyAudio::Sound.open(MONO_TEST_WAV) do |snd|
      snd.read(buf)
    end
    buf.real_size.should == 1000
    buf[99].should > 0
  end

  it "should allow reading into an existing buffer partially" do
    buf = RubyAudio::Buffer.float(1000)
    buf.real_size.should == 0
    RubyAudio::Sound.open(MONO_TEST_WAV) do |snd|
      snd.read(buf, 100)
    end
    buf.real_size.should == 100
    buf[99].should > 0
    buf[100].should == nil
  end

  it "should raise exception for channel count mismatch on read" do
    buf = RubyAudio::Buffer.float(1000, 1)
    lambda {
      RubyAudio::Sound.open(STEREO_TEST_WAV) do |snd|
        snd.read(buf)
      end
    }.should raise_error(RubyAudio::Error, "channel count mismatch: 1 vs 2")
  end

  it "should allow writing to a new sound" do
    in_buf = RubyAudio::Buffer.float(100)
    out_buf = RubyAudio::Buffer.float(100)
    out_info = nil
    RubyAudio::Sound.open(MONO_TEST_WAV) do |snd|
      snd.read(in_buf)
      out_info = snd.info.clone
    end

    RubyAudio::Sound.open(OUT_WAV, 'rw', out_info) do |snd|
      snd.write(in_buf)
      snd.seek(0)
      snd.read(out_buf)
    end

    out_buf[50].should == in_buf[50]
  end

  it "should allow writing to a new sound using <<" do
    in_buf = RubyAudio::Buffer.float(100)
    out_buf = RubyAudio::Buffer.float(100)
    out_info = nil
    RubyAudio::Sound.open(MONO_TEST_WAV) do |snd|
      snd.read(in_buf)
      out_info = snd.info.clone
    end

    RubyAudio::Sound.open(OUT_WAV, 'rw', out_info) do |snd|
      snd << in_buf
      snd.seek(0)
      snd.read(out_buf)
    end

    out_buf[50].should == in_buf[50]
  end
end