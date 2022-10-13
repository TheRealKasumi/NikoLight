# TesLight UI

The TesLight UI is a web-based ReactJS frontend.
It was designed by [BlackBeaver.eu](https://blackbeaver.eu) and programmed by me (TheRealKasumi).
Since it's a ReactJS based project, there is a required build step before it can be used.

## Build the Frontend

To build the frontend, [NodeJS](https://nodejs.org/) and [NPM](https://www.npmjs.com/) are required.
Keep in mind that NPM comes with NodeJS, so you don't need to install it manually.
Open a terminal an navigate into the [web-app](/web-app) folder.

```sh
npm install
npm run build
```

Afterwards there will be [build](/web-app/build) folder.
This contains the ready to use frontend files.
These can be copied to the MicroSD card and used with TesLight.
For more information, check the [build guide](../documentation/build.md).

## Mock Server (For Developers)

There is a minimalistic mock server in the [mock-server](/web-app/mock-server) folder.
It helps you to work on the frontend without having a connected TesLight controller.
Navigate into the folder and run `npm run start` to start the mock server.
When starting the frontend via `npm run start`, all requests are forwarded to the mock server automatically.
