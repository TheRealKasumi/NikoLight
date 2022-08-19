/**
 * @file WebServer.h
 * @author TheRealKasumi
 * @brief Contains a class that handles the webserver and rest api.
 * @version 0.0.1
 * @date 2022-06-27
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdint.h>

#include <HTTP_Method.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include "logging/Logger.h"

namespace TesLight
{
	class WebServer
	{
	public:
		WebServer(const uint16_t port, FS *fileSystem, const String staticContentLocation);
		~WebServer();

		void addRequestHandler(const char *uri, http_method method, ArRequestHandlerFunction handler);
		void addRequestBodyHandler(const char *uri, http_method method, ArRequestHandlerFunction requestHandler, ArBodyHandlerFunction bodyHandler);

	private:
		AsyncWebServer *server;
		FS *fileSystem;
		String staticContentLocation;

		void init();

		void handleNotFound(AsyncWebServerRequest *request);
	};
}

#endif
