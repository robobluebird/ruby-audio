require 'rubygems'
require 'rake'
require 'rake/testtask'
require 'rake/rdoctask'
require 'rake/gempackagetask'

spec = Gem::Specification.new do |s|
  s.name    = 'ruby-audio'
  s.version = '0.2.0'
  s.summary = 'ruby-audio wraps around libsndfile to provide simplified sound reading and writing support to ruby programs'
  s.authors  = ['Hans Fugal <hans@fugal.net>', 'Stephen Augenstein']
  s.email    = 'perl.programmer@gmail.com'
  s.homepage = 'http://github.com/warhammerkid/ruby-audio'

  s.platform     = Gem::Platform::RUBY
  s.has_rdoc     = true
  s.files        = FileList['README.rdoc', 'Rakefile', 'LICENSE', 'TODO', 'examples/**/*.rb', 'examples/**/*.wav', 'lib/**/*.rb', 'test/*.rb', 'test/*.wav', 'ext/sndfile/extconf.rb', 'ext/sndfile/sndfile.i']
  s.require_path = 'lib'
  s.extensions   = ["ext/sndfile/extconf.rb"]
  s.test_files   = Dir[*['test/*.rb']]

  s.add_dependency('narray')
  s.requirements << 'libsndfile (http://www.mega-nerd.com/libsndfile/)'
  s.requirements << 'SWIG (http://www.swig.org/)'
end

desc 'Default: Run the tests'
task :default => :test

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

desc "Run tests"
Rake::TestTask.new do |t|
  t.libs += ['ext/sndfile']
end