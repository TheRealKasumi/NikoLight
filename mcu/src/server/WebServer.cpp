/**
 * @file WebServer.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link WebServer}.
 * @version 0.0.1
 * @date 2022-06-27
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:WebServer"), (String)F("Starting webserver on port ") + String(port) + F("."));

	this->server = new AsyncWebServer(port);
	this->fileSystem = fileSystem;
	this->staticContentLocation = staticContentLocation;

	this->init();

	this->server->begin();

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:WebServer"), F("Webserver running."));
}

/**
 * @brief Destroy the {@link TesLight::WebServer} instance.
 */
TesLight::WebServer::~WebServer()
{
	delete this->server;
}

/**
 * @brief Initialize the {@link TesLight::WebServer} and server static files.
 */
void TesLight::WebServer::init()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("WebServer.cpp:init"), F("Setting handler for not found error."));
	this->server->onNotFound([this](AsyncWebServerRequest *request)
							 { this->handleNotFound(request); });

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("WebServer.cpp:init"), (String)F("Serving static files from: ") + this->staticContentLocation);
	this->server->serveStatic("/", *this->fileSystem, this->staticContentLocation.c_str());
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