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
TL::WebServerManager *TL::RestEndpoint::webServerManager = nullptr;
WebServer *TL::RestEndpoint::webServer = nullptr;
String TL::RestEndpoint::baseUri = F("");

/**
 * @brief Initialize the rest endpoint.
 * @param _webServer instance of {@link TL::WebServerManager}
 * @param _baseUri base uri under which the endpoint is exposed
 */
void TL::RestEndpoint::init(TL::WebServerManager *_webServerManager, String _baseUri)
{
	TL::RestEndpoint::webServerManager = _webServerManager;
	TL::RestEndpoint::webServer = webServerManager->getWebServer();
	TL::RestEndpoint::baseUri = _baseUri;
}

/**
 * @brief Get the {@link TL::WebServerManager} reference.
 * @return {@link TL::WebServerManager} reference
 */
TL::WebServerManager *TL::RestEndpoint::getServerManager()
{
	return TL::RestEndpoint::webServerManager;
}

/**
 * @brief Get the {@link WebServer} reference.
 * @return {@link WebServer} reference
 */
WebServer *TL::RestEndpoint::getServer()
{
	return TL::RestEndpoint::webServer;
}

/**
 * @brief Get the base uri of the endpoint.
 * @return {@link String} containing the base uri
 */
String TL::RestEndpoint::getBaseUri()
{
	return TL::RestEndpoint::baseUri;
}

/**
 * @brief Send a simple json response.
 * @param code http status code
 * @param message status or information message
 */
void TL::RestEndpoint::sendSimpleResponse(const int code, const String &message)
{
	DynamicJsonDocument jsonDoc(1024);
	TL::RestEndpoint::sendJsonDocument(code, message, jsonDoc);
}

/**
 * @brief Send a json document to the client. The status code is determined from the json document itself.
 * @param jsonDocument reference to the json document to send to the client
 */
void TL::RestEndpoint::sendJsonDocument(const int code, const String &message, DynamicJsonDocument &jsonDocument)
{
	jsonDocument[F("status")] = code;
	jsonDocument[F("message")] = message;

	const size_t serializedSize = measureJson(jsonDocument);
	uint8_t *buffer = new uint8_t[serializedSize];

	const size_t length = serializeJson(jsonDocument, buffer, serializedSize);
	webServer->setContentLength(length);

	TL::RestEndpoint::webServer->send_P(code, "application/json", (char *)buffer, length);
	delete[] buffer;
}

/**
 * @brief Deserialize a json string into a dynamic json document.
 * @param jsonDocument reference to the json document
 * @param json string containing the json
 * @return true when successful
 * @return false when there was an error
 */
bool TL::RestEndpoint::parseJsonDocument(DynamicJsonDocument &jsonDocument, const String &json)
{
	DeserializationError error = deserializeJson(jsonDocument, json);
	return error.code() == DeserializationError::Code::Ok;
}
