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
#include "util/InMemoryBinaryFile.h"
#include "logging/Logger.h"

namespace TesLight
{
	class LedConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin(TesLight::Configuration *_configuration, std::function<void()> _configChangedCallback);

	private:
		LedConfigurationEndpoint();

		static TesLight::Configuration *configuration;
		static std::function<void()> configChangedCallback;

		static void getLedConfig(AsyncWebServerRequest *request);
		static void postLedConfig(AsyncWebServerRequest *request);

		static bool validateLedPin(const int ledPin);
		static bool validateLedCount(const int ledCount);
		static bool validateAnimatorType(const int animatorType);
	};
}

#endif
