var fs = require("fs");

fs.readFile('log.txt', 'utf-8', function(err, data) {
	if (err) throw err;

	console.log("data=" + data);
});
console.log("was here");
