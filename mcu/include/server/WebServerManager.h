/**
 * @file WebServerManager.h
 * @author TheRealKasumi
 * @brief Contains a class for managing the synchronious WebServer.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <stdint.h>

#include <HTTP_Method.h>
#include <WebServer.h>
#include <FS.h>

#include "logging/Logger.h"

namespace TesLight
{
	class WebServerManager
	{
	public:
		WebServerManager(const uint16_t port, FS *fileSystem, const String staticContentLocation);
		~WebServerManager();

		WebServer *getWebServer();

		void addRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction handler);
		void addUploadRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction requestHandler, WebServer::THandlerFunction uploadHandler);

		void begin();
		void stop();

		void handleRequest();

	private:
		WebServer *webServer;
		FS *fileSystem;
		String staticContentLocation;

		void init();
		void handleNotFound();
	};
}

#endif
