require 'google/cloud/pubsub'
require 'json'
pubsub = Google::Cloud::Pubsub.new
topic = pubsub.topic 'telemetry'
loop do
  topic.publish "{\"ambientTemp\": #{rand()}, \"beerTemp\": #{rand}}"
  sleep 1
end
