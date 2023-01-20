/**
 * @file WebServerManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::WebServerManager}.
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
#include "server/WebServerManager.h"

bool TL::WebServerManager::initialized = false;
WebServer *TL::WebServerManager::webServer;
FS *TL::WebServerManager::fileSystem;

/**
 * @brief Start the web server manager.
 * @param fileSystem file system to serve static content from
 */
void TL::WebServerManager::begin(FS *fileSystem, const uint16_t port)
{
	TL::WebServerManager::initialized = true;
	TL::WebServerManager::webServer = new WebServer(port);
	TL::WebServerManager::fileSystem = fileSystem;
	TL::WebServerManager::init();
}

/**
 * @brief Stop the web server manager.
 */
void TL::WebServerManager::end()
{
	TL::WebServerManager::initialized = false;
	TL::WebServerManager::webServer->stop();
	delete TL::WebServerManager::webServer;
}

/**
 * @brief Check if the web server manager is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::WebServerManager::isInitialize()
{
	return TL::WebServerManager::initialized;
}

/**
 * @brief Start the web server.
 */
void TL::WebServerManager::startServer()
{
	TL::WebServerManager::webServer->begin();
}

/**
 * @brief Get a reference to the {@link WebServer} instance.
 * @return WebServer* reference to the instance
 */
WebServer *TL::WebServerManager::getWebServer()
{
	return TL::WebServerManager::webServer;
}

/**
 * @brief Add a request handler for a speciffic uri.
 * @param uri uri of the endpoint
 * @param handler handler function
 */
void TL::WebServerManager::addRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction handler)
{
	TL::WebServerManager::webServer->on(uri, method, handler);
}

/**
 * @brief Add a request handler with a body handler for a speciffic uri.
 *
 * @param uri uri of the endpoint
 * @param requestHandler handler function is called after the upload
 * @param uploadHandler handler function for receiving upload data
 */
void TL::WebServerManager::addUploadRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction requestHandler, WebServer::THandlerFunction uploadHandler)
{
	TL::WebServerManager::webServer->on(uri, method, requestHandler, uploadHandler);
}

/**
 * @brief Function must be called regularly to accept new conenctions and handle client.
 */
void TL::WebServerManager::handleRequest()
{
	TL::WebServerManager::webServer->handleClient();
}

/**
 * @brief Initialize the {@link TL::WebServerManager} and start serving static files.
 */
void TL::WebServerManager::init()
{
	const char *headerKeys[1] = {"content-type"};
	webServer->collectHeaders(headerKeys, 1);

	TL::WebServerManager::webServer->onNotFound([]()
												{ TL::WebServerManager::handleNotFound(); });
	TL::WebServerManager::webServer->serveStatic(WEB_SERVER_STATIC_CONTENT, *TL::WebServerManager::fileSystem, WEB_SERVER_STATIC_CONTENT);
	TL::WebServerManager::webServer->on("/", http_method::HTTP_GET, []()
										{TL::WebServerManager::webServer->sendHeader("Location", "/ui/index.html"); TL::WebServerManager::webServer->send(301); });
}

/**
 * @brief Handle not found error.
 */
void TL::WebServerManager::handleNotFound()
{
	if (TL::WebServerManager::webServer->method() == HTTP_OPTIONS)
	{
		TL::WebServerManager::webServer->send(200);
	}
	else
	{
		TL::WebServerManager::webServer->send(404);
	}
}
