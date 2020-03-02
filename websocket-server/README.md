This pulls messages from google pubsub and sends them on via websocket on port 8080 locally.

In production it is deployed at `wss://hombrew-iot.appspot.com`, and can be accessed like this:

```javascript
let socket = new WebSocket("wss://hombrew-iot.appspot.com")
```
