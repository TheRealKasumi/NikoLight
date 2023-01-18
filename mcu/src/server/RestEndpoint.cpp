/**
 * @file RestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains the implementation of a base class for a single REST endpoint.
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
#include "server/RestEndpoint.h"

// Initialize
WebServer *TL::RestEndpoint::webServer;
String TL::RestEndpoint::baseUri;

/**
 * @brief Initialize the rest endpoint.
 * @param _baseUri base uri under which the endpoint is exposed
 */
void TL::RestEndpoint::init(String _baseUri)
{
	TL::RestEndpoint::webServer = TL::WebServerManager::getWebServer();
	TL::RestEndpoint::baseUri = _baseUri;
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
