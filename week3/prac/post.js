var express = require('express');
var app = express();

app.post('/', function (req, res) {
	res.send('Got a POST request');
});

app.listen(4885, function () {
	console.log('Example app listening on port 4885!');
});
