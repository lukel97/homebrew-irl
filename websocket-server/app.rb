require 'google/cloud/pubsub'
require 'websocket-eventmachine-server'

pubsub = Google::Cloud::Pubsub.new
topic = pubsub.topic 'telemetry'
sub = topic.subscription 'websocket'

@socks = []

subscriber = sub.listen do |msg|
  puts "Got message #{msg.data}"
  for ws in @socks
    ws.send msg.data
  end
  msg.acknowledge!
end

port = ENV['PORT'] || 443

EM.run do
  WebSocket::EventMachine::Server.start(:host => '0.0.0.0', :port => port) do |ws|
    ws.onopen do
      puts "Socket opened"
      @socks << ws
    end
    ws.onclose do
      puts "Socket closed"
      @socks -= [ws]
    end
    ws.onerror do |e|
      puts "error #{e}"
    end
  end
  puts "Now listening on port #{port}"
  subscriber.start
end
