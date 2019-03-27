/***
	GET으로 호출
	ex) 2000년 1월 1일, 1시 2분, 30.05도(섭씨)
	 http://54.180.101.233:4885/log?year=2000&month=1&date=1
	 &hour=1&minute=2&temp=30.05
***/

/*
 * mysql 실행, 'me'@'localhost', mydb 데이터베이스 연결
 */
mysql = require('mysql');
var connection = mysql.createConnection({
	host: 'localhost',
	user: 'me',
	password: '',
	database: 'mydb'
})
connection.connect();

/*
 * current_temperatures 테이블에 삽입
 */
function insertCurrTempTable(year, month, date, hour, minute, temp) {
	obj = {};
	obj.year = year;
	obj.month = month;
	obj.date = date;
	obj.hour = hour;
	obj.minute = minute;
	obj.temp = temp;

	var query = connection.query('insert into current_temperatures set ?', obj, function(err, rows, cols) {
		if (err) throw err;
		console.log("database insert ok= %j", obj);
	});
}

var express = require('express'); 
var app = express(); // app은 express의 인스턴스
var fs = require('fs');
var numeral = require('numeral'); // 수의 출력형식

/*
 * GET method route(홈페이지 라우트)
 */
app.get('/', function(req, res) {
	res.send('2019 Capstone Design I, week 4');
});

var seq = 0;
var txt;		// '../*?' 이후 텍스트 저장

/*
 * 
 * GET method route
 * 라우트 경로: 요청(req)을 /log에 일치
 */
app.get('/log', function(req, res) {
	// query를 이용하여 파싱한 값을 table에 insert
	r = req.query;
	console.log('GET %j', r);
	insertCurrTempTable(r.year, r.month, r.date, r.hour, r.minute, r.temp, req.connection.remoteAddress);
	res.end('OK: ' + JSON.stringify(req.query));

	// 파싱한 후, 각 데이터의 형식 변환
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

	// 날짜, 온도(yyyymmdd,hh:mm,temp) 로그, log.txt에 계속 저장
	var logStr = String(year) + String(formMonth) + String(formDate) + ',';
	logStr += String(formHour) + ':' + String(formMinute) + ',' + String(temp);
	fs.appendFile('log.txt', logStr + '\n', function (err) {
		if (err) throw err;
		console.log(logStr);
		res.end(logStr);
	});
	
});

/*
* 텍스트로 출력
*/
app.get('/dump', function(req, res) {
	txt = JSON.stringify(req.query);
	var jData = JSON.parse(txt);	// JSON 파싱으로 구분

	var count = jData.count;		// count='xx'일때 xx값 저장
	
	var array = fs.readFileSync('log.txt').toString().split("\n"); // log.txt의 로그, 배열로 저장
	var lastIdx = array.length - 1;
	var logStr; // 출력할 로그
		
	logStr = "";
	for (var i = lastIdx - count; i < lastIdx; i++) {
		logStr += array[i] + '<br>';
	}

	res.send(logStr); // 로그 출력
	res.end(seq);
});

/*
 * 그래프로 출력
 */
app.get('/graph', function (req, res) {
	console.log('got app.get(graph)');

	var html = fs.readFile('./graph.html', function (err, html) {
		html = " " + html;
		console.log('read file');

		// current_temperatures 테이블 조회
		var qstr = 'select * from current_temperatures';
		connection.query(qstr, function(err, rows, cols) {
			if (err) throw err;
			
			var data = "";
			var comma = "";
			for (var i = 0; i < rows.length; i++) {
				r = rows[i];
				data += comma + "[new Date(" + r.year + "," + r.month + "-" +r.date + "," + r.hour + "," + r.minute + "," + "50)," + r.temp + "]";
				comma = ",";
			}
			
			console.log(data);			
			var header = "data.addColumn('date');"
			header += "data.addColumn('number', 'Temp');"
			html = html.replace("<%HEADER%>", header);
			html = html.replace("<%DATA%>", data);

			res.writeHeader(200, {"Content-Type": "text/html"});
			res.write(html);
			res.end();
		});
	});
});


app.listen(4885, function() {
	console.log('Homework app of week 4 listening on port 4885!');
});
