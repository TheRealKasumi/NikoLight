/**
 * @file index.js
 * @author TheRealKasumi
 * @brief Tool to minify/uglify the TesLight frontend files.
 *
 * @copyright Copyright (c) 2022
 *
 */
const { exit } = require("process");
const fs = require("fs");
const path = require("path");
const { JSDOM } = require("jsdom");
const uglifyhtml = require("html-minifier");
const uglifycss = require("uglifycss");
const uglifyjs = require("uglify-JS");

/**
 * Entry point of the tool.
 * The file name of the main HTML file is expected as command line argument.
 * The HTML file is then loaded and all directly linked JS-, CSS- and image- files are minified/uglified
 * and directly embedded into the one HTML file. This is done because the ESP32 webserver is not able to
 * handle more than a few parallel requests to load the frontend files due to hardware limitations.
 * However modern webbrowser can use many parallel HTTP requests and may make the ESP32 crash.
 * That's why as many files as possible are embedded into a single HTML file.
 */
function main() {
	console.log("This tool will embed all frontend files (it is capable of) into the main HTML file.");

	// Check if the file name to the main HTML file is provided
	if (process.argv.length < 3) {
		console.log("Please provide the file name of the main HTML file as command line argument.");
		exit(1);
	}

	// Get the file name and the file extension
	const fileName = path.basename(process.argv[2]);
	const fileExtension = path.extname(process.argv[2]);
	const workingDir = path.dirname(process.argv[2]) + "/";
	console.log("File name: " + fileName);
	console.log("File extension: " + fileExtension);
	console.log("Directory: " + workingDir);

	// Check if the file extension is HTML
	if (fileExtension !== ".html") {
		console.log('Invalid file extension "' + fileExtension + '". Only HTML files are accepted.');
		exit(2);
	}

	// Minify/uglify the HTML file and embed files where possible
	console.log('Minifying file "' + fileName + ".");
	minifyHtml(fileName, workingDir);
	exit(0);
}

/**
 * Minifiy/uglify an HTML file and embed resources where possible.
 * @param {string} fileName name of the file
 * @param {string} workingDir current working directory
 */
function minifyHtml(fileName, workingDir) {
	// Load the HTML file as text
	console.log('Loading HTML file "' + workingDir + fileName + "...");
	let htmlFile;
	try {
		htmlFile = fs.readFileSync(workingDir + fileName, { encoding: "utf8" });
	} catch (ex) {
		console.log(ex);
		console.log("Failed to load HTML file.");
		exit(3);
	}
	console.log("HTML file loaded.");

	// Parse the HTML DOM
	console.log("Parse HTML DOM...");
	let dom;
	try {
		dom = new JSDOM(htmlFile);
	} catch (ex) {
		console.log(ex);
		console.log("Failed to parse HTML file.");
		exit(4);
	}
	console.log("HTML DOM parsed.");

	// Recursively search the DOM for elements to minify/uglify and embed
	searchHtmlTree(dom.window.document, workingDir);

	// Serialize and minify/uglify the DOM
	const minHtml = uglifyhtml.minify(dom.serialize(), {
		collapseWhitespace: true,
		removeAttributeQuotes: true,
		removeComments: true,
		removeRedundantAttributes: true,
		sortAttributes: true,
		useShortDoctype: true,
		minifyCSS: true,
		minifyJS: true,
	});

	// Save the minified/uglified HTML with embeded content
	console.log("Saving minified/uglified HTML with embeded content...");
	fs.writeFileSync(workingDir + fileName + ".min.html", minHtml);
	console.log("Minified HTML file saved.");
}

/**
 * Recurively walk throught the DOM and minify/uglify/embed content.
 * @param {string} fileName name of the file
 * @param {string} workingDir current working directory
 */
function searchHtmlTree(rootNode, workingDir) {
	// Iterate over all child nodes
	console.log("Iterate over " + rootNode.childNodes.length + " child nodes...");
	for (let i = 0; i < rootNode.childNodes.length; i++) {
		const child = rootNode.childNodes[i];

		// Minify/uglify and embed CSS files
		if (child.nodeName === "LINK" && child.rel === "stylesheet" && child.href) {
			console.log(
				'Found CSS element with href attribute "' + child.href + '". Minifying, uglifying and embedding...'
			);
			child.href = minifyCss(path.basename(child.href), workingDir + path.dirname(child.href) + "/");
			console.log("Processed CSS file.");
		}

		// Embed icon files
		else if (child.nodeName === "LINK" && child.rel === "icon" && child.href) {
			console.log(
				'Found icon element with href attribute "' + child.href + '". Embedding...'
			);
			child.href = base64File(path.basename(child.href), workingDir + path.dirname(child.href) + "/");
			console.log("Processed icon file.");
		}

		// Embed all images with a src attribute
		else if (child.nodeName === "IMG" && child.src) {
			console.log('Found image element with src attribute "' + child.src + '". Embedding...');
			child.src = base64File(path.basename(child.src), workingDir + path.dirname(child.src) + "/");
			console.log("Processed image file.");
		}

		// Minify/uglify and embed JS files
		else if (child.nodeName === "SCRIPT" && child.src) {
			console.log(
				'Found script element with src attribute "' + child.src + '". Minifying, uglifying and embedding...'
			);
			child.src = minifyJs(path.basename(child.src), workingDir + path.dirname(child.src) + "/");
			console.log("Processed JS file.");
		}

		// Recursive call
		searchHtmlTree(child, workingDir);
	}
}

/**
 * Walk throught CSS files and minify/uglify/embed content.
 * @param {string} fileName name of the file
 * @param {string} workingDir current working directory
 * @returns Minified, uglified and encoded CSS that can be embedded
 */
function minifyCss(fileName, workingDir) {
	console.log('Load CSS file "' + workingDir + fileName + '".');
	let cssFile;
	try {
		cssFile = fs.readFileSync(workingDir + fileName, "utf8");
	} catch (ex) {
		console.log(ex);
		console.log("Failed to load CSS file.");
		exit(5);
	}
	console.log("CSS file loaded.");

	// Find all url links
	console.log("Extract all urls...");
	const urlRegex = /url\(.*?\)/gi;
	let urlList = cssFile.match(urlRegex);
	if (!urlList) {
		urlList = [];
	}
	console.log("Found " + urlList.length + " url's.");

	// Iterate over all url's and embed the linked files
	console.log("Embed all files from url's...");
	for (let i = 0; i < urlList.length; i++) {
		let url = urlList[i];
		url = url.replace('url("', "").replace('")', "");

		const base64 = base64File(path.basename(url), workingDir + path.dirname(url) + "/");
		cssFile = cssFile.replace(urlList[i], 'url("' + base64 + '")');
	}
	console.log("All files embedded.");

	// Minify/uglify CSS
	console.log("Minifying/uglifying CSS file...");
	const finalCss = uglifycss.processString(cssFile);
	console.log("CSS minified/uglified.");

	// Encode and return CSS
	return base64String(finalCss, "text/css");
}

/**
 * Minify, uglify and base64 encode JS file to embed it in HTML.
 * @param {string} fileName name of the file
 * @param {string} workingDir current working directory
 * @returns Minified, uglified and encoded JS that can be embedded
 */
function minifyJs(fileName, workingDir) {
	console.log('Load JS file "' + workingDir + fileName + '".');
	let jsFile;
	try {
		jsFile = fs.readFileSync(workingDir + fileName, "utf8");
	} catch (ex) {
		console.log(ex);
		console.log("Failed to load CSS file.");
		exit(6);
	}
	console.log("JS file loaded.");

	// Minify/uglify JS
	console.log("Minifying/uglifying js file...");
	const result = uglifyjs.minify(jsFile);
	if (result.error) {
		console.log("Failed to uglify JS file: " + result.error);
		exit(7);
	}

	// Encode and return CSS
	return base64String(result.code, "text/javascript");
}

/**
 * Load and encode files with base64 encoding so that it can be used as embedded data in HTML.
 * @param {string} fileName name of the file
 * @param {string} workingDir current working directory
 * @returns base64 representation of the file
 */
function base64File(fileName, workingDir) {
	// Get the file extension
	console.log('Base64 encode file "' + workingDir + fileName + '".');
	const fileExtension = path.extname(workingDir + fileName);
	console.log('File extension is "' + fileExtension + '".');

	// Assume the mime type based on the file extension
	console.log("Assuming mime type of the file...");
	let mimeType = "application/octet-stream";
	switch (fileExtension) {
		case ".html":
			mimeType = "text/html";
			break;
		case ".htm":
			mimeType = "text/html";
			break;
		case ".css":
			mimeType = "text/css";
			break;
		case ".js":
			mimeType = "text/javascript";
			break;
		case ".png":
			mimeType = "image/png";
			break;
		case ".jpg":
			mimeType = "image/jpg";
			break;
		case ".jpeg":
			mimeType = "image/jpeg";
			break;
		case ".gif":
			mimeType = "image/gif";
			break;
		case ".bmp":
			mimeType = "image/bmp";
			break;
	}
	console.log('Assumes mime type is "' + mimeType + '".');

	console.log('Load file "' + workingDir + fileName + '" for base64 encoding...');
	try {
		const base64 = fs.readFileSync(workingDir + fileName, { encoding: "base64" });
		console.log("file was encoded with base64.");
		return "data:" + mimeType + ";base64," + base64;
	} catch (err) {
		console.log(err);
		console.log('Failed to encode file "' + workingDir + fileName + '".');
		exit(8);
	}
}

/**
 * Base64 encode a string so that it can be used as embedded data in HTML.
 * @param {string} data string to encode
 * @param {string} mimeType mime type
 * @returns base64 representation of the string
 */
function base64String(data, mimeType) {
	const base64 = Buffer.from(data).toString("base64");
	return "data:" + mimeType + ";base64," + base64;
}

main();
