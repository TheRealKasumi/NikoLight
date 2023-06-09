/**
 * @file RestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains the implementation of a base class for a single REST endpoint.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "server/RestEndpoint.h"

// Initialize
WebServer *NL::RestEndpoint::webServer;
String NL::RestEndpoint::baseUri;

/**
 * @brief Initialize the rest endpoint.
 * @param _baseUri base uri under which the endpoint is exposed
 */
void NL::RestEndpoint::init(String _baseUri)
{
	NL::RestEndpoint::webServer = NL::WebServerManager::getWebServer();
	NL::RestEndpoint::baseUri = _baseUri;
}

/**
 * @brief Get the {@link WebServer} reference.
 * @return {@link WebServer} reference
 */
WebServer *NL::RestEndpoint::getServer()
{
	return NL::RestEndpoint::webServer;
}

/**
 * @brief Get the base uri of the endpoint.
 * @return {@link String} containing the base uri
 */
String NL::RestEndpoint::getBaseUri()
{
	return NL::RestEndpoint::baseUri;
}

/**
 * @brief Send a simple json response.
 * @param code http status code
 * @param message status or information message
 */
void NL::RestEndpoint::sendSimpleResponse(const int code, const String &message)
{
	DynamicJsonDocument jsonDoc(1024);
	NL::RestEndpoint::sendJsonDocument(code, message, jsonDoc);
}

/**
 * @brief Send a json document to the client. The status code is determined from the json document itself.
 * @param jsonDocument reference to the json document to send to the client
 */
void NL::RestEndpoint::sendJsonDocument(const int code, const String &message, DynamicJsonDocument &jsonDocument)
{
	jsonDocument[F("status")] = code;
	jsonDocument[F("message")] = message;

	const size_t serializedSize = measureJson(jsonDocument);
	uint8_t *buffer = new uint8_t[serializedSize];

	const size_t length = serializeJson(jsonDocument, buffer, serializedSize);
	webServer->setContentLength(length);

	NL::RestEndpoint::webServer->send_P(code, "application/json", (char *)buffer, length);
	delete[] buffer;
}

/**
 * @brief Deserialize a json string into a dynamic json document.
 * @param jsonDocument reference to the json document
 * @param json string containing the json
 * @return true when successful
 * @return false when there was an error
 */
bool NL::RestEndpoint::parseJsonDocument(DynamicJsonDocument &jsonDocument, const String &json)
{
	DeserializationError error = deserializeJson(jsonDocument, json);
	return error.code() == DeserializationError::Code::Ok;
}
