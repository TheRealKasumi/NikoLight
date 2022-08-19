/**
 * @file Configuration.h
 * @author TheRealKasumi
 * @brief Contains a class to load and save the (runtime) configuration.
 * @version 0.0.1
 * @date 2022-07-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <FS.h>

#include "configuration/SystemConfiguration.h"
#include "util/BinaryFile.h"
#include "logging/Logger.h"

namespace TesLight
{
	class Configuration
	{
	public:
		struct LogConfig
		{
			TesLight::Logger::LogLevel logLevel;
		};

		struct WiFiConfig
		{
			String accessPointSsid;
			String accessPointPassword;
			uint8_t accessPointChannel;
			bool accessPointHidden;
			uint8_t accessPointMaxConnections;
			String wifiSsid;
			String wifiPassword;
		};

		struct LedConfig
		{
			uint8_t ledPin;
			uint16_t ledCount;
			uint8_t type;
			uint8_t speed;
			uint16_t offset;
			uint8_t brightness;
			bool reverse;
			uint8_t customField[NUM_ANIMATOR_CUSTOM_FIELDS];
		};

		Configuration(FS *fileSystem, const String fileName);
		~Configuration();

		TesLight::Configuration::LogConfig getLogConfig();
		void setLogConfig(TesLight::Configuration::LogConfig logConfig);

		TesLight::Configuration::WiFiConfig getWiFiConfig();
		void setWiFiConfig(TesLight::Configuration::WiFiConfig wifiConfig);

		TesLight::Configuration::LedConfig getLedConfig(const uint8_t index);
		void setLedConfig(const TesLight::Configuration::LedConfig ledConfig, const uint8_t index);

		void loadDefaults();
		bool load();
		bool save();

	private:
		FS *fileSystem;
		String fileName;

		TesLight::Configuration::LogConfig logConfig;
		TesLight::Configuration::WiFiConfig wifiConfig;
		TesLight::Configuration::LedConfig ledConfig[NUM_LED_DRIVERS];
	};
}

#endif