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
#include <memory>
#include <HTTP_Method.h>
#include <WebServer.h>
#include <FS.h>
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"

namespace TL
{
	class WebServerManager
	{
	public:
		WebServerManager(FS *fileSystem);
		~WebServerManager();

		WebServer *getWebServer();

		void addRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction handler);
		void addUploadRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction requestHandler, WebServer::THandlerFunction uploadHandler);

		void begin();
		void stop();

		void handleRequest();

	private:
		std::unique_ptr<WebServer> webServer;
		FS *fileSystem;

		void init();
		void handleNotFound();
	};
}

#endif
