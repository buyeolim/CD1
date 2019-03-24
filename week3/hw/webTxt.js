var express = require('express');
var app = express();
var fs = require("fs");
var numeral = require('numeral'); // npm install numeral , 자리수 형식

app.get('/', function(req, res) {
	res.send('2019 Capstone Design I, week 3');
});

var seq = 0;
var txt;		// '../*?' 이후 텍스트 저장
var i = 0;

app.get('/log', function(req, res) {
	txt = JSON.stringify(req.query);
	var jData = JSON.parse(txt);

	var year = jData.year;
	var month = jData.month;
	var formMonth = numeral(month).format('00');
	var date = jData.date;
	var formDate = numeral(date).format('00');
	var	hour = jData.hour;
	var formHour = numeral(hour).format('00');
	var minute = jData.minute;
	var formMinute = numeral(minute).format('00');
	var	temp = jData.temp;

	var logStr = String(year) + String(formMonth) + String(formDate) + ',';
	logStr += String(formHour) + ':' + String(formMinute) + ',' + String(temp);

	// 날짜, 온도(yyyymmdd,hh:mm,temp) 로그, log.txt에 계속 저장
	fs.appendFile('log.txt', logStr + '\n', function (err) {
		if (err) throw err;
		console.log(logStr);
		res.end(logStr);
	});
	
});

app.get('/dump', function(req, res) {
	txt = JSON.stringify(req.query);
	var jData = JSON.parse(txt);	// JSON 파싱으로 구분

	var count = jData.count;		// count='xx'일때 xx값 저장
	
	var array = fs.readFileSync('log.txt').toString().split("\n"); // log.txt의 로그, 배열로 저장
	var lastIdx = array.length - 1;
	var logStr; // 출력할 로그
		
	logStr = "";
	for (i = lastIdx - count; i < lastIdx; i++) {
		logStr += array[i] + '<br>';
	}

	res.send(logStr); // 로그 출력
	res.end(seq);
});

app.listen(4885, function() {
	console.log('Homework app of week 3 listening on port 4885!');
});
