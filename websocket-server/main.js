// let socket = new WebSocket("wss://hombrew-iot.appspot.com")
let socket = new WebSocket("ws://lukelau.me:8080")
socket.onopen = () => console.log('Connected')
socket.onclose = () => console.log('Closed')
socket.onerror = console.error
socket.onmessage = function (event) {
	let body = JSON.parse(event.data)
	document.getElementById("beerTemp").innerHTML = "Beer temperature: " + body.beerTemp
	document.getElementById("ambientTemp").innerHTML = "Ambient temperature: " + body.ambientTemp
}
