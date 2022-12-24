/**
 * @file SystemConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to configure the system settings.
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
#include "logging/Logger.h"

namespace TL
{
	class SystemConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin(TL::Configuration *_configuration, std::function<bool()> _configChangedCallback);

	private:
		SystemConfigurationEndpoint();

		static TL::Configuration *configuration;
		static std::function<bool()> configChangedCallback;

		static void getSystemConfig();
		static void postSystemConfig();

		static bool validateConfiguration(const JsonObject &jsonObject);
		static bool isInRange(const long value, const long min, const long max);
		static bool validateMinMax(const long min, const long max);
	};
}

#endif
