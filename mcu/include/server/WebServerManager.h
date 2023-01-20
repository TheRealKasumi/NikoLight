/**
 * @file WebServerManager.h
 * @author TheRealKasumi
 * @brief Contains a class for managing the synchronious WebServer.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
 *
 * This project, including hardware and software, is provided "as is". There is no warranty
 * of any kind, express or implied, including but not limited to the warranties of fitness
 * for a particular purpose and noninfringement. TheRealKasumi (https://github.com/TheRealKasumi)
 * is holding ownership of this project. You are free to use, modify, distribute and contribute
 * to this project for private, non-commercial purposes. It is granted to include this hardware
 * and software into private, non-commercial projects. However, the source code of any project,
 * software and hardware that is including this project must be public and free to use for private
 * persons. Any commercial use is hereby strictly prohibited without agreement from the owner.
 * By contributing to the project, you agree that the ownership of your work is transferred to
 * the project owner and that you lose any claim to your contribute work. This copyright and
 * license note applies to all files of this project and must not be removed without agreement
 * from the owner.
 *
 */
#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <stdint.h>
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
		static void begin(FS *fileSystem, const uint16_t port);
		static void end();
		static bool isInitialize();

		static void startServer();
		static WebServer *getWebServer();

		static void addRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction handler);
		static void addUploadRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction requestHandler, WebServer::THandlerFunction uploadHandler);

		static void handleRequest();

	private:
		WebServerManager();

		static bool initialized;
		static WebServer *webServer;
		static FS *fileSystem;

		static void init();
		static void handleNotFound();
	};
}

#endif
