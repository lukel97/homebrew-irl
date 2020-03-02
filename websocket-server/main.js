let socket = new WebSocket("wss://0.0.0.0:8080")
socket.onerror = console.log
socket.onmessage = function (event) {
	let body = JSON.parse(event.data)
	document.getElementById("beerTemp").innerHTML = "Beer temperature: " + body.beerTemp
	document.getElementById("ambientTemp").innerHTML = "Ambient temperature: " + body.ambientTemp
}
