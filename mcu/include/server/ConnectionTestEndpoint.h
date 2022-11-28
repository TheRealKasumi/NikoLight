/**
 * @file ConnectionTestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to test the connection to the controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef CONNECTION_TEST_ENDPOINT_H
#define CONNECTION_TEST_ENDPOINT_H

#include "server/RestEndpoint.h"

namespace TesLight
{
	class ConnectionTestEndpoint : public RestEndpoint
	{
	public:
		static void begin();

	private:
		ConnectionTestEndpoint();

		static void handleConnectionTest();
	};
}

#endif
