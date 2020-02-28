require 'google/cloud/pubsub'
require 'websocket-eventmachine-server'

pubsub = Google::Cloud::Pubsub.new
topic = pubsub.topic 'telemetry'
sub = topic.subscription 'websocket'

@socks = []

subscriber = sub.listen do |msg|
  for ws in @socks
    ws.send msg.data
  end
  msg.acknowledge!
end

subscriber.start

EventMachine.run do
  WebSocket::EventMachine::Server.start(:host => '0.0.0.0', :port => 443) do |ws|
    ws.onopen do
      @socks << ws
    end
    ws.onclose do
      @socks -= [ws]
    end
  end
end
