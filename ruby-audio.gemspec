# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-audio}
  s.version = "1.4.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Stephen Augenstein"]
  s.date = %q{2010-12-23}
  s.email = %q{perl.programmer@gmail.com}
  s.extensions = ["ext/extconf.rb"]
  s.files = ["README.rdoc", "Rakefile", "LICENSE", "lib/ruby-audio/buffer.rb", "lib/ruby-audio/sound.rb", "lib/ruby-audio/sound_info.rb", "lib/ruby-audio.rb", "spec/buffer_spec.rb", "spec/sound_info_spec.rb", "spec/sound_spec.rb", "spec/spec_helper.rb", "spec/spec.opts", "spec/data/what.wav", "spec/data/what2.wav", "spec/data/what.mp3", "ext/extconf.rb", "ext/ra_buffer.c", "ext/ra_sound.c", "ext/ra_soundinfo.c", "ext/rubyaudio_ext.c", "ext/ra_buffer.h", "ext/ra_sound.h", "ext/ra_soundinfo.h"]
  s.homepage = %q{http://github.com/warhammerkid/ruby-audio}
  s.rdoc_options = ["--line-numbers", "--main", "README.rdoc", "ext/ra_buffer.c", "ext/ra_sound.c", "ext/ra_soundinfo.c", "ext/rubyaudio_ext.c", "README.rdoc"]
  s.require_paths = ["lib"]
  s.requirements = ["libsndfile (http://www.mega-nerd.com/libsndfile/)"]
  s.rubygems_version = %q{1.3.7}
  s.summary = %q{ruby-audio wraps around libsndfile to provide simplified sound reading and writing support to ruby programs}
  s.test_files = ["spec/buffer_spec.rb", "spec/sound_info_spec.rb", "spec/sound_spec.rb"]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
