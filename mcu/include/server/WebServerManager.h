/**
 * @file WebServerManager.h
 * @author TheRealKasumi
 * @brief Contains a class for managing the synchronious WebServer.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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

namespace NL
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
