/**
 * @file Configuration.h
 * @author TheRealKasumi
 * @brief Contains a class to load and save the (runtime) configuration.
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
#include "sensor/LightSensor.h"

namespace TesLight
{
	class Configuration
	{
	public:
		struct SystemConfig
		{
			// Log config
			TesLight::Logger::LogLevel logLevel;

			// Light sensor config
			TesLight::LightSensor::LightSensorMode lightSensorMode;
			uint16_t lightSensorThreshold;
			uint16_t lightSensorMinValue;
			uint16_t lightSensorMaxValue;

			// Power config
			uint8_t systemPowerLimit; // W
		};

		struct LedConfig
		{
			// Pin Configuration
			uint8_t ledPin;
			uint16_t ledCount;

			// Animation Configuration
			uint8_t type;
			uint8_t speed;
			uint16_t offset;
			uint8_t brightness;
			bool reverse;
			uint8_t fadeSpeed;
			uint8_t customField[ANIMATOR_NUM_CUSTOM_FIELDS];

			// Power config
			uint8_t ledVoltage;			  // Voltage * 10
			uint8_t ledChannelCurrent[3]; // mA
		};

		struct WiFiConfig
		{
			// Access point configuration
			String accessPointSsid;
			String accessPointPassword;
			uint8_t accessPointChannel;
			bool accessPointHidden;
			uint8_t accessPointMaxConnections;

			// WiFi network configuration
			String wifiSsid;
			String wifiPassword;
		};

		Configuration(FS *fileSystem, const String fileName);
		~Configuration();

		TesLight::Configuration::SystemConfig getSystemConfig();
		void setSystemConfig(TesLight::Configuration::SystemConfig systemConfig);

		TesLight::Configuration::LedConfig getLedConfig(const uint8_t index);
		void setLedConfig(const TesLight::Configuration::LedConfig ledConfig, const uint8_t index);

		TesLight::Configuration::WiFiConfig getWiFiConfig();
		void setWiFiConfig(TesLight::Configuration::WiFiConfig wifiConfig);

		void loadDefaults();
		bool load();
		bool save();

	private:
		FS *fileSystem;
		String fileName;
		uint16_t configurationVersion;

		TesLight::Configuration::SystemConfig systemConfig;
		TesLight::Configuration::LedConfig ledConfig[LED_NUM_ZONES];
		TesLight::Configuration::WiFiConfig wifiConfig;

		uint16_t getSimpleHash();
		uint16_t getSimpleStringHash(const String input);
	};
}

#endif