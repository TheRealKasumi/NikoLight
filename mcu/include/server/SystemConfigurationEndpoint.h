/**
 * @file SystemConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to configure the system settings.
 * @version 0.0.1
 * @date 2022-07-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SYSTEM_CONFIGURATION_ENDPOINT_H
#define SYSTEM_CONFIGURATION_ENDPOINT_H

#include <functional>

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "util/InMemoryBinaryFile.h"
#include "logging/Logger.h"

extern "C"
{
#include "crypto/base64.h"
}

namespace TesLight
{
	class SystemConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin(TesLight::Configuration *_configuration, std::function<void()> _configChangedCallback);

	private:
		SystemConfigurationEndpoint();

		static TesLight::Configuration *configuration;
		static std::function<void()> configChangedCallback;

		static void getSystemConfig(AsyncWebServerRequest *request);
		static void postSystemConfig(AsyncWebServerRequest *request);
	};
}

#endif
