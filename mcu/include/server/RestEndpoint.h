/**
 * @file RestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a base class to model a single REST endpoint.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef REST_ENDPOINT_H
#define REST_ENDPOINT_H

#include "server/WebServer.h"

namespace TesLight
{
	class RestEndpoint
	{
	public:
		static void init(TesLight::WebServer *_webServer, String _baseUri);

		static TesLight::WebServer *getWebServer();
		static String getBaseUri();

	private:
		static TesLight::WebServer *webServer;
		static String baseUri;

		RestEndpoint();
	};
}

#endif
