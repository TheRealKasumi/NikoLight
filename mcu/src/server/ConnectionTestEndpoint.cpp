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
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::ConnectionTestEndpoint::begin()
{
	TL::ConnectionTestEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("connection_test")).c_str(), http_method::HTTP_GET, TL::ConnectionTestEndpoint::handleConnectionTest);
}

/**
 * @brief Handler function for the connection test.
 */
void TL::ConnectionTestEndpoint::handleConnectionTest()
{
	TL::ConnectionTestEndpoint::sendSimpleResponse(200, F("I am doing alright, thanks for asking :3 !"));
}