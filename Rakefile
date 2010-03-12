require 'rubygems'
require 'rake'
require 'rake/rdoctask'
require 'rake/gempackagetask'
require 'spec/rake/spectask'

spec = Gem::Specification.new do |s|
  s.name    = 'ruby-audio'
  s.version = '1.0.0'
  s.summary = 'ruby-audio wraps around libsndfile to provide simplified sound reading and writing support to ruby programs'
  s.authors  = ['Stephen Augenstein']
  s.email    = 'perl.programmer@gmail.com'
  s.homepage = 'http://github.com/warhammerkid/ruby-audio'

  s.platform     = Gem::Platform::RUBY
  s.has_rdoc     = true
  s.files        = FileList['README.rdoc', 'Rakefile', 'LICENSE', 'lib/**/*.rb', 'spec/**/*.{rb,opts,wav,mp3}', 'ext/extconf.rb', 'ext/*.{c,h}']
  s.require_path = 'lib'
  s.extensions   = ["ext/extconf.rb"]
  s.test_files   = Dir[*['spec/**/*_spec.rb']]

  s.requirements << 'libsndfile (http://www.mega-nerd.com/libsndfile/)'
end

desc 'Default: Run the tests'
task :default => :spec

# Rake gem & package routines
Rake::GemPackageTask.new spec do |pkg|
  pkg.need_tar = true
  pkg.need_zip = true
end

desc "Generate documentation"
Rake::RDocTask.new(:rdoc) do |rdoc|
  rdoc.rdoc_dir = 'rdoc'
  rdoc.title    = 'ruby-audio'
  rdoc.options << '--line-numbers' << '--main' << 'README.rdoc'
  rdoc.rdoc_files.include('README.rdoc')
  rdoc.rdoc_files.include('lib/**/*.rb')
end

Spec::Rake::SpecTask.new do |t|
  t.spec_opts = ['--options', 'spec/spec.opts']
end