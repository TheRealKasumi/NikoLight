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
#include "server/WebServerManager.h"
#include "util/Base64Util.h"

namespace TesLight
{
	class RestEndpoint
	{
	public:
		static void init(TesLight::WebServerManager *_webServerManager, String _baseUri);

		static TesLight::WebServerManager *getServerManager();
		static WebServer *getServer();
		static String getBaseUri();

	private:
		RestEndpoint();

	protected:
		static TesLight::WebServerManager *webServerManager;
		static WebServer *webServer;
		static String baseUri;
	};
}

#endif
