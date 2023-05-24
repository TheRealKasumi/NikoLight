/**
 * @file Configuration.h
 * @author TheRealKasumi
 * @brief Contains a class to load and save the (runtime) configuration.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
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

#include <stdint.h>
#include <vector>
#include <tuple>
#include <WString.h>
#include <FS.h>

#include "configuration/SystemConfiguration.h"
#include "util/BinaryFile.h"
#include "hardware/AudioUnit.h"

namespace NL
{
	class Configuration
	{
	public:
		enum class Error
		{
			OK,						   // No error
			ERROR_PROFILE_NOT_FOUND,   // No profile with the given name was found
			ERROR_PROFILE_NAME_EXISTS, // The profile name is already in use
			ERROR_PROFILE_IS_ACTIVE,   // The profile is currently used and can not be deleted
			ERROR_TOO_MANY_PROFILES,   // Too many profiles to load
			ERROR_OUT_OF_BOUNDS,	   // The index is out of bounds
			ERROR_FILE_OPEN,		   // Failed to open file
			ERROR_FILE_READ,		   // Failed to read file
			ERROR_FILE_WRITE,		   // Failed to write file
			ERROR_FILE_VERSION,		   // Unmatching file version
			ERROR_FILE_HASH			   // Unmatching file hash
		};

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
			uint8_t fanMode;						 // Mode of the cooling fan
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
			uint8_t dataSource;											// Data source of the animation
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

		struct AudioUnitConfig
		{
			uint16_t noiseThreshold;													// Threshold to filter out static noise
			std::pair<uint16_t, uint16_t> frequencyBandIndex[AUDIO_UNIT_NUM_BANDS];		// Frequency band start and end bin indices
			NL::AudioUnit::PeakDetectorConfig peakDetectorConfig[AUDIO_UNIT_NUM_BANDS]; // Settings for the peak detector
		};

		struct UIConfiguration
		{
			String firmware;
			String language;
			String theme;
			bool expertMode;
		};

		struct Profile
		{
			String name;										   // Name of the profile
			NL::Configuration::SystemConfig systemConfig;		   // System configuration of the profile
			NL::Configuration::LedConfig ledConfig[LED_NUM_ZONES]; // LED configuration of the profile
			NL::Configuration::UIConfiguration uiConfiguration;	   // UI configuration of the profile
		};

		static void begin(FS *fileSystem, const String fileName);
		static void end();
		static bool isInitialized();

		static size_t getProfileCount();
		static NL::Configuration::Error getProfileNameByIndex(const size_t profileIndex, String &profileName);
		static NL::Configuration::Error getProfile(const String &profileName, NL::Configuration::Profile &profile);
		static NL::Configuration::Error createProfile(const String &profileName);
		static NL::Configuration::Error cloneProfile(const String &sourceName, const String &destinationName);
		static NL::Configuration::Error renameProfile(const String &profileName, const String &newProfileName);
		static NL::Configuration::Error deleteProfile(const String &profileName);

		static String getActiveProfile();
		static NL::Configuration::Error setActiveProfile(const String &profileName);

		static NL::Configuration::SystemConfig getSystemConfig();
		static void setSystemConfig(NL::Configuration::SystemConfig &systemConfig);

		static NL::Configuration::Error getLedConfig(const uint8_t zoneIndex, NL::Configuration::LedConfig &ledConfig);
		static NL::Configuration::Error setLedConfig(const uint8_t zoneIndex, const NL::Configuration::LedConfig &ledConfig);

		static NL::Configuration::WiFiConfig getWiFiConfig();
		static void setWiFiConfig(NL::Configuration::WiFiConfig &wifiConfig);

		static NL::Configuration::MotionSensorCalibration getMotionSensorCalibration();
		static void setMotionSensorCalibration(const NL::Configuration::MotionSensorCalibration &calibration);

		static NL::Configuration::AudioUnitConfig getAudioUnitConfig();
		static void setAudioUnitConfig(const NL::Configuration::AudioUnitConfig &audioUnitConfig);

		static NL::Configuration::UIConfiguration getUIConfiguration();
		static void setUIConfiguration(const NL::Configuration::UIConfiguration &uiConfiguration);

		static void loadDefaults();
		static NL::Configuration::Error load();
		static NL::Configuration::Error save();

	private:
		Configuration();

		static bool initialized;
		static FS *fileSystem;
		static String fileName;
		static uint16_t configurationVersion;

		static size_t activeProfile;
		static std::vector<NL::Configuration::Profile> profiles;
		static NL::Configuration::WiFiConfig wifiConfig;
		static NL::Configuration::MotionSensorCalibration motionSensorCalibration;
		static NL::Configuration::AudioUnitConfig audioUnitConfig;

		static NL::Configuration::Error loadProfileDefaults(const size_t profileIndex);
		static NL::Configuration::Error getProfileIndexByName(const String &profileName, size_t &profileIndex);

		static uint16_t getSimpleHash();
		static uint16_t getSimpleStringHash(const String input);
	};
}

#endif