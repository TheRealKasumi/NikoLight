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
			TesLight::Logger::LogLevel logLevel;					// Logging level
			TesLight::LightSensor::LightSensorMode lightSensorMode; // Mode of the light sensor
			uint16_t lightSensorThreshold;							// Threshold value to turn on/off the LEDs
			uint16_t lightSensorMinValue;							// Minimum value for automatic brightness adjustment
			uint16_t lightSensorMaxValue;							// Maximum value for automatic brightness adjustment
			uint8_t regulatorPowerLimit;							// Limit in W
			uint8_t regulatorHighTemperature;						// Temp in °C where brightness is reduced
			uint8_t regulatorCutoffTemperature;						// Temp in °C where LEDs are turned off
			uint8_t fanMinPwmValue;									// Minimum pwm value output to the fan (stall guard)
			uint8_t fanMaxPwmValue;									// Maximum pwm value output to the fan
			uint8_t fanMinTemperature;								// Minimum temp in °C where the fan starts
			uint8_t fanMaxTemperature;								// Maximum temp in °C to run at maximum speed
		};

		struct LedConfig
		{
			uint8_t ledPin;									 // Physical pin for the LED output
			uint16_t ledCount;								 // Number of LEDs
			uint8_t type;									 // Type of the animation
			uint8_t speed;									 // Speed of the animation
			uint16_t offset;								 // Offset for the animation
			uint8_t brightness;								 // Brightness of the LED channel
			bool reverse;									 // Reverse the animation
			uint8_t fadeSpeed;								 // Fading speed when turning on/off
			uint8_t customField[ANIMATOR_NUM_CUSTOM_FIELDS]; // Custom fields for the animation
			uint8_t ledVoltage;								 // Voltage of the LED x10
			uint8_t ledChannelCurrent[3];					 // Current for each LED channel per LED in mA
		};

		struct WiFiConfig
		{
			String accessPointSsid;			   // SSID for the access point
			String accessPointPassword;		   // Password for the access point
			uint8_t accessPointChannel;		   // Channel for the access point
			bool accessPointHidden;			   // Hide the access point
			uint8_t accessPointMaxConnections; // Maximum number of connection to the access point
			String wifiSsid;				   // SSID of a WiFi network
			String wifiPassword;			   // Password of a WiFi network
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