# Minifier

## What is this tool used for and why is it required

The frontend (web-app) is a browser based UI that is used to configure and control TesLight.
The required files are served by the TesLight microcontroller from the inserted microSD card.
Since the TesLight controller (based on an ESP32) has a very limited capabilities when acting as a web server, it can not handle many requests in parallel.
However modern browsers can use many parallel requests when loading a web page like the TesLight UI.
The microcontroller might not be able to handle these (depending on the browser) and could get overloaded with requests.

For this the minifier is used.
It will load the main HTML file of the frontend and search for css, js and image references.
These usually lead to parallel requests by the browser.
Instead of referencing these files, the minifier will embed them into a single HTML file (as far as possible).
This reduces the number of requests to 1 in a best case scenario and reduces the load on the microcontroller.

## Usage

To run the minification use the following commands:

```sh
npm install
npm run minify
```

This is assuming that the main HTML file is located in `../index.html`.
