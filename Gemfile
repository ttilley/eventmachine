source :rubygems
gemspec

group :test do
  gem "rake", "~> 0.9.2"
end

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
