/**
 * @file WebServerManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::WebServerManager}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/WebServerManager.h"

/**
 * @brief Create a new instance of {@link TL::WebServerManager}.
 * @param fileSystem instance of {@link FS}
 */
TL::WebServerManager::WebServerManager(FS *fileSystem)
{
	this->webServer.reset(new WebServer(WEB_SERVER_PORT));
	this->fileSystem = fileSystem;
	this->init();
}

/**
 * @brief Destroy the {@link TL::WebServerManager} instance and shut down the web server.
 */
TL::WebServerManager::~WebServerManager()
{
	this->stop();
}

/**
 * @brief Get a reference to the {@link WebServer} instance.
 * @return WebServer* reference to the instance
 */
WebServer *TL::WebServerManager::getWebServer()
{
	return this->webServer.get();
}

/**
 * @brief Add a request handler for a speciffic uri.
 *
 * @param uri uri of the endpoint
 * @param handler handler function
 */
void TL::WebServerManager::addRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction handler)
{
	this->webServer->on(uri, method, handler);
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
	this->webServer->on(uri, method, requestHandler, uploadHandler);
}

/**
 * @brief Start the web server. Before starting the server, all endpoints must be added.
 */
void TL::WebServerManager::begin()
{
	this->webServer->begin();
}

/**
 * @brief Stop the web server.
 */
void TL::WebServerManager::stop()
{
	this->webServer->stop();
}

/**
 * @brief Function must be called regularly to accept new conenctions and handle client.
 */
void TL::WebServerManager::handleRequest()
{
	this->webServer->handleClient();
}

/**
 * @brief Initialize the {@link TL::WebServerManager} and start serving static files.
 */
void TL::WebServerManager::init()
{
	const char *headerKeys[1] = {"content-type"};
	webServer->collectHeaders(headerKeys, 1);

	this->webServer->onNotFound([this](){ this->handleNotFound(); });
	this->webServer->serveStatic(WEB_SERVER_STATIC_CONTENT, *this->fileSystem, WEB_SERVER_STATIC_CONTENT);
	this->webServer->on("/", http_method::HTTP_GET, [this](){this->webServer->sendHeader("Location", "/ui/index.html"); this->webServer->send(301); });
}

/**
 * @brief Handle not found error.
 */
void TL::WebServerManager::handleNotFound()
{
	if (this->webServer->method() == HTTP_OPTIONS)
	{
		this->webServer->send(200);
	}
	else
	{
		this->webServer->send(404);
	}
}
