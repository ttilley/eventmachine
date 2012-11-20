source :rubygems
gemspec

# is this an RVM oddity?
gem 'psych', :platforms => :mri_19

group :development, :test do
  gem 'pry'
  gem 'pry-doc'
  gem 'pry-pretty-numeric'

  platforms :mri_19 do
    if Gem.ruby_version.release < Gem::Version.new("2.0.0")
      gem 'pry-debugger'
      gem 'pry-exception_explorer'
      gem 'pry-stack_explorer'
    end
  end
end
