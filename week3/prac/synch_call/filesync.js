var fs = require("fs");

data = fs.readFileSync('log.txt', 'utf-8');
console.log("data=" + data);
