/**
 * @file ConnectionTestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to test the connection to the controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/ConnectionTestEndpoint.h"

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServer}.
 */
void TesLight::ConnectionTestEndpoint::begin()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register Connection Test Endpoint."));
	getWebServer()->addRequestHandler((getBaseUri() + F("connection_test")).c_str(), http_method::HTTP_GET, TesLight::ConnectionTestEndpoint::handleConnectionTest);
}

/**
 * @brief Handler function for the connectiont est.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::ConnectionTestEndpoint::handleConnectionTest(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Received request to connection test endpoint."));
	request->send(200, F("application/text"), (String)F("Hey there, I am running. Current runtime: ") + String(millis()));
}