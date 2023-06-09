/**
 * @file RestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a base class for a single REST endpoint.
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
#ifndef REST_ENDPOINT_H
#define REST_ENDPOINT_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include "server/WebServerManager.h"

namespace NL
{
	class RestEndpoint
	{
	public:
		static void init(String _baseUri);

		static WebServer *getServer();
		static String getBaseUri();

	private:
		RestEndpoint();

	protected:
		static WebServer *webServer;
		static String baseUri;

		static void sendSimpleResponse(const int code, const String &message);
		static void sendJsonDocument(const int code, const String &message, DynamicJsonDocument &jsonDocument);
		static bool parseJsonDocument(DynamicJsonDocument &jsonDocument, const String &json);
	};
}

#endif
