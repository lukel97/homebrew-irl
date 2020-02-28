let socket = new WebSocket("ws://0.0.0.0:8080")
socket.onmessage = function (event) {
	let body = JSON.parse(event.data)
	document.getElementById("beerTemp").innerHTML = "Beer temperature: " + body.beerTemp
	document.getElementById("ambientTemp").innerHTML = "Ambient temperature: " + body.ambientTemp
}
