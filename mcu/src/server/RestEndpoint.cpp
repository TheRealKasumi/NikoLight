/**
 * @file RestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains the implementation of a base class for a single REST endpoint.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/RestEndpoint.h"

// Initialize
TesLight::WebServerManager *TesLight::RestEndpoint::webServerManager = nullptr;
WebServer *TesLight::RestEndpoint::webServer = nullptr;
String TesLight::RestEndpoint::baseUri = F("");

/**
 * @brief Initialize the rest endpoint.
 * @param _webServer instance of {@link TesLight::WebServerManager}
 * @param _baseUri base uri under which the endpoint is exposed
 */
void TesLight::RestEndpoint::init(TesLight::WebServerManager *_webServerManager, String _baseUri)
{
	TesLight::RestEndpoint::webServerManager = _webServerManager;
	TesLight::RestEndpoint::webServer = webServerManager->getWebServer();
	TesLight::RestEndpoint::baseUri = _baseUri;
}

/**
 * @brief Get the {@link TesLight::WebServerManager} reference.
 * @return {@link TesLight::WebServerManager} reference
 */
TesLight::WebServerManager *TesLight::RestEndpoint::getServerManager()
{
	return TesLight::RestEndpoint::webServerManager;
}

/**
 * @brief Get the {@link WebServer} reference.
 * @return {@link WebServer} reference
 */
WebServer *TesLight::RestEndpoint::getServer()
{
	return TesLight::RestEndpoint::webServer;
}

/**
 * @brief Get the base uri of the endpoint.
 * @return {@link String} containing the base uri
 */
String TesLight::RestEndpoint::getBaseUri()
{
	return TesLight::RestEndpoint::baseUri;
}
