/**
 * @file WebServerManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::WebServerManager}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/WebServerManager.h"

/**
 * @brief Create a new instance of {@link TesLight::WebServerManager}.
 *
 * @param port port to run the server on
 * @param fileSystem instance of {@link FS}
 * @param staticContentLocation location of the static content on the sd card
 */
TesLight::WebServerManager::WebServerManager(const uint16_t port, FS *fileSystem, const String staticContentLocation)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Starting webserver on port ") + String(port) + F("."));
	this->webServer = new WebServer(port);
	this->fileSystem = fileSystem;
	this->staticContentLocation = staticContentLocation;
	this->init();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Webserver running."));
}

/**
 * @brief Destroy the {@link TesLight::WebServerManager} instance and shut down the web server.
 */
TesLight::WebServerManager::~WebServerManager()
{
	this->stop();
	delete this->webServer;
}

/**
 * @brief Get a reference to the {@link WebServer} instance.
 * @return WebServer* reference to the instance
 */
WebServer *TesLight::WebServerManager::getWebServer()
{
	return this->webServer;
}

/**
 * @brief Add a request handler for a speciffic uri.
 *
 * @param uri uri of the endpoint
 * @param handler handler function
 */
void TesLight::WebServerManager::addRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction handler)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Adding request handler for \"") + uri + F("\"."));
	this->webServer->on(uri, method, handler);
}

/**
 * @brief Add a request handler with a body handler for a speciffic uri.
 *
 * @param uri uri of the endpoint
 * @param requestHandler handler function is called after the upload
 * @param uploadHandler handler function for receiving upload data
 */
void TesLight::WebServerManager::addUploadRequestHandler(const char *uri, http_method method, WebServer::THandlerFunction requestHandler, WebServer::THandlerFunction uploadHandler)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Adding upload request handler for \"") + uri + F("\"."));
	this->webServer->on(uri, method, requestHandler, uploadHandler);
}

/**
 * @brief Start the web server. Before starting the server, all endpoints must be added.
 */
void TesLight::WebServerManager::begin()
{
	this->webServer->begin();
}

/**
 * @brief Stop the web server.
 */
void TesLight::WebServerManager::stop()
{
	this->webServer->stop();
}

/**
 * @brief Function must be called regularly to accept new conenctions and handle client.
 */
void TesLight::WebServerManager::handleRequest()
{
	this->webServer->handleClient();
}

/**
 * @brief Initialize the {@link TesLight::WebServerManager} and start serving static files.
 */
void TesLight::WebServerManager::init()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Setting handler for not found error."));
	this->webServer->onNotFound([this]()
								{ this->handleNotFound(); });

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Serving static files from: ") + this->staticContentLocation);
	this->webServer->serveStatic("/web-app/", *this->fileSystem, this->staticContentLocation.c_str());
	this->webServer->on("/", http_method::HTTP_GET, [this]()
						{this->webServer->sendHeader("Location", "/web-app/index.html"); this->webServer->send(301); });
}

/**
 * @brief Handle not found error.
 */
void TesLight::WebServerManager::handleNotFound()
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