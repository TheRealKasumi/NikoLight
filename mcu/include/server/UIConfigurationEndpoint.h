/**
 * @file UIConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to read and save UI related settings.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef UI_CONFIGURATION_ENDPOINT_H
#define UI_CONFIGURATION_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "logging/Logger.h"

namespace TL
{
	class UIConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin();

	private:
		UIConfigurationEndpoint();

		static void getUIConfig();
		static void postUIConfig();

		static bool validateUIConfig(const JsonObject &jsonObject);
	};
}

#endif
