/**
 * @file WebServerManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::WebServerManager}.
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
#include "server/WebServerManager.h"

bool NL::WebServerManager::initialized = false;
WebServer *NL::WebServerManager::webServer;
FS *NL::WebServerManager::fileSystem;

/**
 * @brief Start the web server manager.
 * @param fileSystem file system to serve static content from
 */
void NL::WebServerManager::begin(FS *fileSystem, const uint16_t port)
{
	NL::WebServerManager::initialized = true;
	NL::WebServerManager::webServer = new WebServer(port);
	NL::WebServerManager::fileSystem = fileSystem;
	NL::WebServerManager::init();
}

/**
 * @brief Stop the web server manager.
 */
void NL::WebServerManager::end()
{
	NL::WebServerManager::initialized = false;
	NL::WebServerManager::webServer->stop();
	delete NL::WebServerManager::webServer;
}

/**
 * @brief Check if the web server manager is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::WebServerManager::isInitialize()
{
	return NL::WebServerManager::initialized;
}

/**
 * @brief Start the web server.
 */
void NL::WebServerManager::startServer()
{
	NL::WebServerManager::webServer->begin();
}

/**
 * @brief Get a reference to the {@link WebServer} instance.
 * @return WebServer* reference to the instance
 */
WebServer *NL::WebServerManager::getWebServer()
{
	return NL::WebServerManager::webServer;
}

/**
 * @brief Add a request handler for a speciffic uri.
 * @param uri uri of the endpoint
 * @param handler handler function
 */
void NL::WebServerManager::addRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction handler)
{
	NL::WebServerManager::webServer->on(uri, method, handler);
}

/**
 * @brief Add a request handler with a body handler for a speciffic uri.
 *
 * @param uri uri of the endpoint
 * @param requestHandler handler function is called after the upload
 * @param uploadHandler handler function for receiving upload data
 */
void NL::WebServerManager::addUploadRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction requestHandler, WebServer::THandlerFunction uploadHandler)
{
	NL::WebServerManager::webServer->on(uri, method, requestHandler, uploadHandler);
}

/**
 * @brief Function must be called regularly to accept new conenctions and handle client.
 */
void NL::WebServerManager::handleRequest()
{
	NL::WebServerManager::webServer->handleClient();
}

/**
 * @brief Initialize the {@link NL::WebServerManager} and start serving static files.
 */
void NL::WebServerManager::init()
{
	const char *headerKeys[1] = {"content-type"};
	webServer->collectHeaders(headerKeys, 1);

	NL::WebServerManager::webServer->onNotFound([]()
												{ NL::WebServerManager::handleNotFound(); });
	NL::WebServerManager::webServer->serveStatic(WEB_SERVER_STATIC_CONTENT, *NL::WebServerManager::fileSystem, WEB_SERVER_STATIC_CONTENT);
	NL::WebServerManager::webServer->on("/", http_method::HTTP_GET, []()
										{NL::WebServerManager::webServer->sendHeader("Location", "/ui/index.html"); NL::WebServerManager::webServer->send(301); });
}

/**
 * @brief Handle not found error.
 */
void NL::WebServerManager::handleNotFound()
{
	if (NL::WebServerManager::webServer->method() == HTTP_OPTIONS)
	{
		NL::WebServerManager::webServer->send(200);
	}
	else
	{
		NL::WebServerManager::webServer->send(404);
	}
}
