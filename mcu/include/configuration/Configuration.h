/**
 * @file Configuration.h
 * @author TheRealKasumi
 * @brief Contains a class to load and save the (runtime) configuration.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <FS.h>

#include "configuration/SystemConfiguration.h"
#include "configuration/IniFile.h"

namespace TesLight
{
	typedef struct LedDriverConfig
	{
		uint8_t ledPin;
		uint16_t ledCount;
	} LedDriverConfig;

	typedef struct LedAnimatorConfig
	{
		uint8_t type;
		uint8_t speed;
		uint16_t offset;
		uint8_t brightness;
		bool reverse;
		uint8_t customField[10];
	} LedAnimatorConfig;

	class Configuration
	{
	public:
		Configuration(FS *fileSystem, const String fileName);
		~Configuration();

		String getApSSID();
		void setApSSID(const String ssid);

		uint8_t getApChannel();
		void setApChannel(const uint8_t apChannel);

		bool getApHidden();
		void setApHidden(const bool apHidden);

		uint8_t getApMaxConnections();
		void setApMaxConnections(const uint8_t apMaxConnections);

		String getApPassword();
		void setApPassword(const String password);

		String getWifiSSID();
		void setWifiSSID(const String ssid);

		String getWifiPassword();
		void setWifiPassword(const String password);

		TesLight::LedDriverConfig getLedDriverConfig(const uint8_t index);
		void setLedDriverConfig(const TesLight::LedDriverConfig ledDriverConfig, const uint8_t index);

		TesLight::LedAnimatorConfig getLedAnimatorConfig(const uint8_t index);
		void setLedAnimatorConfig(const TesLight::LedAnimatorConfig ledAnimatorConfig, const uint8_t index);

		bool load(bool loadDefaultIfNotExist = true);
		bool save();

	private:
		FS *fileSystem;
		String fileName;

		String apSSID;
		String apPassword;
		uint8_t apChannel;
		bool apHidden;
		uint8_t apMaxConnections;

		String wifiSSID;
		String wifiPassword;

		TesLight::LedDriverConfig ledDriverConfig[NUM_LED_DRIVERS];
		TesLight::LedAnimatorConfig ledAnimatorConfig[NUM_LED_DRIVERS];

		int *parseIntArray(const String array, const uint8_t len);
		String intArrayToString(const int *array, const uint8_t len);
	};
}

#endif