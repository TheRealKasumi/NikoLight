/**
 * @file LedConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to configure the LED settings.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LED_CONFIGURATION_ENDPOINT_H
#define LED_CONFIGURATION_ENDPOINT_H

#include <functional>
#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"

namespace TL
{
	class LedConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin(TL::Configuration *_configuration, std::function<bool()> _configChangedCallback);

	private:
		LedConfigurationEndpoint();

		static TL::Configuration *configuration;
		static std::function<bool()> configChangedCallback;

		static void getLedConfig();
		static void postLedConfig();

		static bool validateLedZone(const JsonObject &jsonObject, const uint8_t index);
		static bool isValidPin(const uint8_t pinNumber);
		static bool isInRange(const long value, const long min, const long max);
		static bool isInRange(const float value, const float min, const float max);
	};
}

#endif
