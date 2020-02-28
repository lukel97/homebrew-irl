FROM ruby:2.6

# install dependencies
WORKDIR /usr/src/app
COPY Gemfile ./
RUN bundle install

# copy over code
COPY . ./

CMD ["ruby", "app.rb"]
