var express = require('express');
var app = express();

app.get('/', function (req, res) {
	res.send('Hello World');
});

var seq = 0;
app.get('/log', function(req, res) {
	console.log("%j", req.query)
	res.end(seq++);
});

app.listen(4885, function () {
	console.log('Example app listening on port 4885!');
});
