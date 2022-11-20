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
#include "util/InMemoryBinaryFile.h"
#include "logging/Logger.h"

namespace TesLight
{
	class SystemConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin(TesLight::Configuration *_configuration, std::function<bool()> _configChangedCallback);

	private:
		SystemConfigurationEndpoint();

		static TesLight::Configuration *configuration;
		static std::function<bool()> configChangedCallback;

		static void getSystemConfig();
		static void postSystemConfig();
		static bool validateMinMax(const uint16_t min, const uint16_t max);
		static bool validateLogLevel(const uint8_t value);
		static bool validateLightSensorMode(const uint8_t value);
		static bool validateLightSensorValue(const uint16_t value);
		static bool validateRegulatorPowerLimit(const uint8_t value);
		static bool validateRegulatorHighTemperature(const uint8_t value);
		static bool validateRegulatorCutoffTemperature(const uint8_t value);
		static bool validateMinFanTemperature(const uint8_t value);
		static bool validateMaxFanTemperature(const uint8_t value);
	};
}

#endif
