$("#NUKE_EVIDENCE").click(function(){
    $.ajax({
        type: "GET",
        url: "nuke/",
        success: function(d){
            var data = JSON.parse(d);
            console.log(data["message"]);
        },
    });
});

// let socket = new WebSocket("ws://lukelau.me:8080")
let socket = new WebSocket("ws://35.184.74.223:443")
socket.onopen = () => $("#live").fadeIn()
socket.onclose = () => $("#live").fadeOut()
socket.onerror = console.error
socket.onmessage = function (event) {
    $("#timestamp").text("just now");
    let body = JSON.parse(event.data)
    $("#beer-temp").text(body.beerTemp + "C")
    $("#ambient-temp").text(body.ambientTemp + "C")
    $("#alcohol-amount").text(body.gasPerc)
}

$.get("temp_chart_json", function(data) {
    var ctx = $("#tempChart").get(0).getContext("2d");
    new Chart(ctx, {
        type: 'line', data: data, options:  {
            maintainAspectRatio: false,
            responsive: false
        }
    });
});

$.get("alcohol_chart_json", function(data) {
    var ctx = $("#alcoholChart").get(0).getContext("2d");
    new Chart(ctx, {
        type: 'line', data: data, options:  {
            maintainAspectRatio: false,
            responsive: false
        }
    });
});