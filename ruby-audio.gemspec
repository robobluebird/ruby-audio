# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.license     = "GPL-2.0"
  s.name        = 'ruby-audio'
  s.version     = '1.6.2'
  s.platform    = Gem::Platform::RUBY
  s.authors     = ['Stephen Augenstein', 'Zachary Schroeder']
  s.email       = ['perl.programmer@gmail.com', 'schroza@gmail.com']
  s.homepage    = 'https://github.com/robobluebird/ruby-audio'
  s.summary     = 'libsndfile wrapper for ruby'
  s.description = 'ruby-audio wraps around libsndfile to provide simplified sound reading and writing support to ruby programs'
  s.files         = Dir['ruby-audio.gemspec', 'README.rdoc', 'LICENSE', 'Rakefile', 'lib/**/*.rb', 'spec/**/*.{rb,opts,wav,mp3}', 'ext/**/*.{c,h,rb}']
  s.extensions    = Dir["ext/ruby_audio/extconf.rb"]
end
