/***
	GET으로 호출
	ex) 2000년 1월 1일, 1시 2분, 30.05도(섭씨)
	 http://54.180.101.233:4885/log?year=2000&month=1&date=1
	 &hour=1&minute=2&temp=30.05
***/

/*
 * mysql 실행, 'me'@'localhost', mydb 데이터베이스 연결
 */
var user = require('./user'); 
mysql = require('mysql');
var connection = mysql.createConnection({
	host: user.host,
	user: user.user,
	password: user.password,
	database: user.database
})
connection.connect();

qstr = 'SELECT * FROM current_temperatures';
connection.query(qstr, function(err, rows, cols) {
	if (err) throw err;
	
	for (var i = 0; i < rows.length; i++) {
		console.log(rows[i]);
	}

});

