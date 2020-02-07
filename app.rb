require 'google/cloud/pubsub'
require 'sinatra'

pubsub = Google::Cloud::Pubsub.new
topic = pubsub.topic 'homebrew-events'
subscription = topic.subscription 'my-subscription'

get '/' do
  topic.publish 'hello world'
  msgs = subscription.pull
  msgs.map(&:data)
end
