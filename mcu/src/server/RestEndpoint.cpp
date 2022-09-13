/**
 * @file RestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a class that represents a single REST endpoint.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/RestEndpoint.h"

// Initialize
TesLight::WebServer *TesLight::RestEndpoint::webServer = nullptr;
String TesLight::RestEndpoint::baseUri = F("");

/**
 * @brief Initialize the rest endpoint.
 * @param _webServer instance of {@link TesLight::WebServer}
 * @param _baseUri base uri under which the endpoint is exposed
 */
void TesLight::RestEndpoint::init(TesLight::WebServer *_webServer, String _baseUri)
{
	webServer = _webServer;
	baseUri = _baseUri;
}

/**
 * @brief Get the {@link TesLight::WebServer} reference.
 * @return {@link TesLight::WebServer} reference
 */
TesLight::WebServer *TesLight::RestEndpoint::getWebServer()
{
	return webServer;
}

/**
 * @brief Get the base uri of the endpoint.
 * @return {@link String} containing the base uri
 */
String TesLight::RestEndpoint::getBaseUri()
{
	return baseUri;
}
