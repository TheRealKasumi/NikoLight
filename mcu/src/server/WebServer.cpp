/**
 * @file WebServer.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link WebServer}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/WebServer.h"

/**
 * @brief Create a new instance of {@link TesLight::WebServer}.
 *
 * @param port port to run the server on
 * @param fileSystem instance of {@link FS}
 * @param staticContentLocation location of the static content on the sd card
 */
TesLight::WebServer::WebServer(const uint16_t port, FS *fileSystem, const String staticContentLocation)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Starting webserver on port ") + String(port) + F("."));

	this->server = new AsyncWebServer(port);
	this->fileSystem = fileSystem;
	this->staticContentLocation = staticContentLocation;

	this->init();

	this->server->begin();

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Webserver running."));
}

/**
 * @brief Destroy the {@link TesLight::WebServer} instance.
 */
TesLight::WebServer::~WebServer()
{
	delete this->server;
}

/**
 * @brief Add a request handler for a speciffic uri.
 *
 * @param uri uri of the endpoint
 * @param handler handler function
 */
void TesLight::WebServer::addRequestHandler(const char *uri, http_method method, ArRequestHandlerFunction handler)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Adding request handler for \"") + uri + F("\"."));
	server->on(uri, method, handler);
}

/**
 * @brief Add a request handler with a body handler for a speciffic uri.
 *
 * @param uri uri of the endpoint
 * @param requestHandler handler function for the request, in case no body is provided
 * @param bodyHandler handler function for receiving body data
 */
void TesLight::WebServer::addRequestBodyHandler(const char *uri, http_method method, ArRequestHandlerFunction requestHandler, ArBodyHandlerFunction bodyHandler)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Adding request body handler for \"") + uri + F("\"."));
	server->on(uri, method, requestHandler, nullptr, bodyHandler);
}

/**
 * @brief Initialize the {@link TesLight::WebServer} and server static files.
 */
void TesLight::WebServer::init()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Setting handler for not found error."));
	this->server->onNotFound([this](AsyncWebServerRequest *request)
							 { this->handleNotFound(request); });

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Serving static files from: ") + this->staticContentLocation);
	this->server->serveStatic("/", *this->fileSystem, this->staticContentLocation.c_str())
		.setDefaultFile("index.html")
		.setCacheControl("max-age=10");
}

/**
 * @brief Handle not found error.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::WebServer::handleNotFound(AsyncWebServerRequest *request)
{
	if (request->method() == HTTP_OPTIONS)
	{
		request->send(200);
	}
	else
	{
		request->send(404);
	}
}