/**
 * Create a minimal mock server for the frontend.
 */
const express = require("express");
const bodyParser = require("body-parser");
const app = express();
const port = 3001;

/**
 * Mock data for the frontend.
 */
let data = {
	systemConfiguration: "AQEeAB4AAAgKMgwMDA==",
	ledConfiguration:
		"DUYAADIKADIABQAAAAAAAAAAAAAOAgAAMgoAMgAFAAAAAAAAAAAAAA8EAAAyCgAyAAUAAAAAAAAAAAAAEAQAADIKADIABQAAAAAAAAAAAAARBAAAMgoAMgAFAAAAAAAAAAAAABUEAAAyCgAyAAUAAAAAAAAAAAAA",
	wifiConfiguration: "CABUZXNMaWdodAoAVGVzTGlnaHRQVwEAAQAAAAA=",
	logData: "00:00:00:000 [INFO] (src/mock) Some fake log data.\n",
};

/**
 * Parse the url-encoded body.
 */
app.use(bodyParser.urlencoded({ extended: true }));

/**
 * Get the system configuration.
 */
app.get("/api/config/system", (req, res) => {
	console.log(`Get system configuration: ${data.systemConfiguration}`);
	res.send(data.systemConfiguration);
});

/**
 * Post the system configuration
 */
app.post("/api/config/system", (req, res) => {
	data.systemConfiguration = req.body.data;
	console.log(`Set new system configuration: ${data.systemConfiguration}`);
	res.status(202).send();
});

/**
 * Get the LED configuration.
 */
app.get("/api/config/led", (req, res) => {
	console.log(`Get LED configuration: ${data.ledConfiguration}`);
	res.send(data.ledConfiguration);
});

/**
 * Post the LED configuration
 */
app.post("/api/config/led", (req, res) => {
	data.ledConfiguration = req.body.data;
	console.log(`Set new LED configuration: ${data.ledConfiguration}`);
	res.status(202).send();
});

/**
 * Get the WiFi configuration.
 */
app.get("/api/config/wifi", (req, res) => {
	console.log(`Get wifi configuration: ${data.wifiConfiguration}`);
	res.send(data.wifiConfiguration);
});

/**
 * Post the WiFi configuration
 */
app.post("/api/config/wifi", (req, res) => {
	data.wifiConfiguration = req.body.data;
	console.log(`Set new wifi configuration: ${data.wifiConfiguration}`);
	res.status(202).send();
});

/**
 * Get the log size.
 */
app.get("/api/log/size", (req, res) => {
	const logSize = data.logData.length * 5000;
	console.log(`Get log size: ${logSize}`);
	res.send(logSize.toString());
});

/**
 * Get the log data.
 */
app.get("/api/log", (req, res) => {
	let expandedLogData = "";
	for (let i = 0; i < 5000; i++) {
		expandedLogData = expandedLogData.concat(data.logData);
	}

	const start = parseInt(req.query.start);
	const count = parseInt(req.query.count);

	if (start < 0 || count < 0 || start + count > expandedLogData.length) {
		console.log("Invalid request to read the log, returning 400.");
		res.status(400).send();
		return;
	}

	let reply = "";
	for (let i = start; i < start + count; i++) {
		reply = reply.concat(expandedLogData.charAt(i));
	}

	console.log(`Get log with a length of ${count} bytes.`);
	res.send(reply);
});

/**
 * Clear the log data.
 */
app.patch("/api/log", (req, res) => {
	res.send();
});

app.listen(port, () => {
	console.log(`Mock server is listening on port ${port}.`);
});
