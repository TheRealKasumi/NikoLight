/**
 * @file Configuration.h
 * @author TheRealKasumi
 * @brief Contains a class to load and save the (runtime) configuration.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
 *
 * This project, including hardware and software, is provided "as is". There is no warranty
 * of any kind, express or implied, including but not limited to the warranties of fitness
 * for a particular purpose and noninfringement. TheRealKasumi (https://github.com/TheRealKasumi)
 * is holding ownership of this project. You are free to use, modify, distribute and contribute
 * to this project for private, non-commercial purposes. It is granted to include this hardware
 * and software into private, non-commercial projects. However, the source code of any project,
 * software and hardware that is including this project must be public and free to use for private
 * persons. Any commercial use is hereby strictly prohibited without agreement from the owner.
 * By contributing to the project, you agree that the ownership of your work is transferred to
 * the project owner and that you lose any claim to your contribute work. This copyright and
 * license note applies to all files of this project and must not be removed without agreement
 * from the owner.
 *
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <FS.h>

#include "configuration/SystemConfiguration.h"
#include "util/BinaryFile.h"
#include "logging/Logger.h"

namespace TL
{
	class Configuration
	{
	public:
		struct SystemConfig
		{
			uint8_t logLevel;						 // Logging level
			uint8_t lightSensorMode;				 // Mode of the light sensor
			uint8_t lightSensorThreshold;			 // Threshold value to turn on/off the LEDs
			uint8_t lightSensorMinAmbientBrightness; // Minimum brightness of the ambient
			uint8_t lightSensorMaxAmbientBrightness; // Maximum brightness of the ambient
			uint8_t lightSensorMinLedBrightness;	 // Minimum brightness of the LEDs for brightnes control
			uint8_t lightSensorMaxLedBrightness;	 // Maximum brightness of the LEDs for brightness control
			uint8_t lightSensorDuration;			 // Time after which the lights are turning off when using the motion sensor (x5 seconds)
			uint8_t regulatorPowerLimit;			 // Limit in W
			uint8_t regulatorHighTemperature;		 // Temp in °C where brightness is reduced
			uint8_t regulatorCutoffTemperature;		 // Temp in °C where LEDs are turned off
			uint8_t fanMinPwmValue;					 // Minimum pwm value output to the fan (stall guard)
			uint8_t fanMaxPwmValue;					 // Maximum pwm value output to the fan
			uint8_t fanMinTemperature;				 // Minimum temp in °C where the fan starts
			uint8_t fanMaxTemperature;				 // Maximum temp in °C to run at maximum speed
		};

		struct LedConfig
		{
			uint8_t ledPin;												// Physical pin for the LED output
			uint16_t ledCount;											// Number of LEDs
			uint8_t type;												// Type of the animation
			uint8_t speed;												// Speed of the animation
			uint16_t offset;											// Offset for the animation
			uint8_t brightness;											// Brightness of the LED channel
			bool reverse;												// Reverse the animation
			uint8_t fadeSpeed;											// Fading speed when turning on/off
			uint8_t animationSettings[ANIMATOR_NUM_ANIMATION_SETTINGS]; // Custom settings for each animation
			float ledVoltage;											// Voltage of the LEDs
			uint8_t ledChannelCurrent[3];								// Current for each LED channel per LED in mA
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

		struct MotionSensorCalibration
		{
			int16_t accXRaw;  // Raw x acceleration
			int16_t accYRaw;  // Raw y acceleration
			int16_t accZRaw;  // Raw z acceleration
			int16_t gyroXRaw; // Raw x rotation
			int16_t gyroYRaw; // Raw y rotation
			int16_t gyroZRaw; // Raw z rotation
			float accXG;	  // X acceleration in g
			float accYG;	  // Y acceleration in g
			float accZG;	  // Z acceleration in g
			float gyroXDeg;	  // X rotation in deg/s
			float gyroYDeg;	  // Y rotation in deg/s
			float gyroZDeg;	  // Z rotation in deg/s
		};

		struct UIConfiguration
		{
			String firmware;
			String language;
			String theme;
			bool expertMode;
		};

		Configuration(FS *fileSystem, const String fileName);
		~Configuration();

		TL::Configuration::SystemConfig getSystemConfig();
		void setSystemConfig(TL::Configuration::SystemConfig &systemConfig);

		TL::Configuration::LedConfig getLedConfig(const uint8_t index);
		void setLedConfig(const TL::Configuration::LedConfig &ledConfig, const uint8_t index);

		TL::Configuration::WiFiConfig getWiFiConfig();
		void setWiFiConfig(TL::Configuration::WiFiConfig &wifiConfig);

		TL::Configuration::MotionSensorCalibration getMotionSensorCalibration();
		void setMotionSensorCalibration(const TL::Configuration::MotionSensorCalibration &calibration);

		TL::Configuration::UIConfiguration getUIConfiguration();
		void setUIConfiguration(const TL::Configuration::UIConfiguration &uiConfiguration);

		void loadDefaults();
		bool load();
		bool save();

	private:
		FS *fileSystem;
		String fileName;
		uint16_t configurationVersion;

		TL::Configuration::SystemConfig systemConfig;
		TL::Configuration::LedConfig ledConfig[LED_NUM_ZONES];
		TL::Configuration::WiFiConfig wifiConfig;
		TL::Configuration::MotionSensorCalibration motionSensorCalibration;
		TL::Configuration::UIConfiguration uiConfiguration;

		uint16_t getSimpleHash();
		uint16_t getSimpleStringHash(const String input);
	};
}

#endif