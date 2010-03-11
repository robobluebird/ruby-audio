require File.dirname(__FILE__) + '/spec_helper.rb'

describe RubyAudio::Buffer do
  it "should initialize properly" do
    buf = RubyAudio::Buffer.new('float', 100, 2)
    buf.channels.should == 2
    buf.size.should == 100
    buf.real_size.should == 0
    buf.type.should == :float
  end

  it "should support pretty typed constructors" do
    lambda {
      [:short, :int, :float, :double].each do |type|
        buf = RubyAudio::Buffer.send(type, 100)
      end
    }.should_not raise_error
  end

  it "should allow [] access" do
    buf = RubyAudio::Buffer.float(100, 2)
    buf[0] = [0.5, 0.3]
    buf[0].should == [0.5, 0.3]
  end

  it "should return nil on out-of-bounds [] access" do
    buf = RubyAudio::Buffer.float(100)
    buf[101].should == nil
    buf[-1].should == nil
  end

  it "should truncate invalid real size" do
    buf = RubyAudio::Buffer.float(100)
    buf.real_size = 101
    buf.real_size.should == 100
  end
end