/**
 * @file RestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a base class for a single REST endpoint.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef REST_ENDPOINT_H
#define REST_ENDPOINT_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include "server/WebServerManager.h"

namespace TL
{
	class RestEndpoint
	{
	public:
		static void init(TL::WebServerManager *_webServerManager, String _baseUri);

		static TL::WebServerManager *getServerManager();
		static WebServer *getServer();
		static String getBaseUri();

	private:
		RestEndpoint();

	protected:
		static TL::WebServerManager *webServerManager;
		static WebServer *webServer;
		static String baseUri;

		static void sendSimpleResponse(const int code, const String &message);
		static void sendJsonDocument(const int code, const String &message, DynamicJsonDocument &jsonDocument);
		static bool parseJsonDocument(DynamicJsonDocument &jsonDocument, const String &json);
	};
}

#endif
