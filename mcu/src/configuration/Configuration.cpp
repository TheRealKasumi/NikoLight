/**
 * @file Configuration.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TL::Configuration}.
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
#include "configuration/Configuration.h"

/**
 * @brief Create a new instance of {@link TL::Configuration}.
 * @param fileSystem pointer to a {@link FS}
 * @param fileName file name of the configuration file
 */
TL::Configuration::Configuration(FS *fileSystem, const String fileName)
{
	this->fileSystem = fileSystem;
	this->fileName = fileName;
	this->configurationVersion = CONFIGURATION_FILE_VERSION;
	this->loadDefaults();
}

/**
 * @brief Destroy the {@link TL::Configuration} instance.
 */
TL::Configuration::~Configuration()
{
}

/**
 * @brief Get the System config.
 * @return instance of {@link TL::Configuration::SystemConfig}
 */
TL::Configuration::SystemConfig TL::Configuration::getSystemConfig()
{
	return this->systemConfig;
}

/**
 * @brief Set the System config.
 * @param SystemConfig config to set
 */
void TL::Configuration::setSystemConfig(TL::Configuration::SystemConfig &systemConfig)
{
	this->systemConfig = systemConfig;
}

/**
 * @brief Get the {@link TL::Configuration::LedConfig}.
 * @param index index of the config
 * @return instance of {TL::Configuration::LedConfig}
 */
TL::Configuration::LedConfig TL::Configuration::getLedConfig(const uint8_t index)
{
	return this->ledConfig[index];
}

/**
 * @brief Set the {@link TL::Configuration::LedConfig}.
 * @param ledConfig instance of {@link TL::Configuration::LedConfig}
 * @param index index of the config
 */
void TL::Configuration::setLedConfig(const TL::Configuration::LedConfig &ledConfig, const uint8_t index)
{
	this->ledConfig[index] = ledConfig;
}

/**
 * @brief Get the WiFi config.
 * @return instance of {@link TL::Configuration::WiFiConfig}
 */
TL::Configuration::WiFiConfig TL::Configuration::getWiFiConfig()
{
	return this->wifiConfig;
}

/**
 * @brief Set the WiFi config.
 * @param wifionfig config to set
 */
void TL::Configuration::setWiFiConfig(TL::Configuration::WiFiConfig &wifiConfig)
{
	this->wifiConfig = wifiConfig;
}

/**
 * @brief Get the motion sensor calibration data.
 * @return calibration data of the motion sensor
 */
TL::Configuration::MotionSensorCalibration TL::Configuration::getMotionSensorCalibration()
{
	return this->motionSensorCalibration;
}

/**
 * @brief Set the motion sensor calibration data.
 * @param calibration calibration data of the motion sensor
 */
void TL::Configuration::setMotionSensorCalibration(const TL::Configuration::MotionSensorCalibration &calibration)
{
	this->motionSensorCalibration = calibration;
}

/**
 * @brief Get the UI configuration.
 * @return UI configuration
 */
TL::Configuration::UIConfiguration TL::Configuration::getUIConfiguration()
{
	return this->uiConfiguration;
}

/**
 * @brief Set the UI configuration.
 * @param uiConfiguration configuration of the UI
 */
void TL::Configuration::setUIConfiguration(const TL::Configuration::UIConfiguration &uiConfiguration)
{
	this->uiConfiguration = uiConfiguration;
}

/**
 * @brief Load the default configuration.
 */
void TL::Configuration::loadDefaults()
{
	// System config
	this->systemConfig.logLevel = LOG_DEFAULT_LEVEL;
	this->systemConfig.lightSensorMode = LIGHT_SENSOR_DEFAULT_MODE;
	this->systemConfig.lightSensorThreshold = LIGHT_SENSOR_DEFAULT_THRESHOLD;
	this->systemConfig.lightSensorMinAmbientBrightness = LIGHT_SENSOR_DEFAULT_MIN_AMBIENT;
	this->systemConfig.lightSensorMaxAmbientBrightness = LIGHT_SENSOR_DEFAULT_MAX_AMBIENT;
	this->systemConfig.lightSensorMinLedBrightness = LIGHT_SENSOR_DEFAULT_MIN_LED;
	this->systemConfig.lightSensorMaxLedBrightness = LIGHT_SENSOR_DEFAULT_MAX_LED;
	this->systemConfig.lightSensorDuration = LIGHT_SENSOR_DEFAULT_DURATION;
	this->systemConfig.regulatorPowerLimit = REGULATOR_POWER_LIMIT * REGULATOR_COUNT;
	this->systemConfig.regulatorHighTemperature = REGULATOR_HIGH_TEMP;
	this->systemConfig.regulatorCutoffTemperature = REGULATOR_CUT_OFF_TEMP;
	this->systemConfig.fanMinPwmValue = FAN_PWM_MIN;
	this->systemConfig.fanMaxPwmValue = FAN_PWM_MAX;
	this->systemConfig.fanMinTemperature = FAN_TEMP_MIN;
	this->systemConfig.fanMaxTemperature = FAN_TEMP_MAX;

	// LED config
	const uint8_t ledPins[LED_NUM_ZONES] = LED_DEFAULT_OUTPUT_PINS;
	const uint8_t ledCounts[LED_NUM_ZONES] = LED_DEFAULT_COUNTS;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		this->ledConfig[i].ledPin = ledPins[i];
		this->ledConfig[i].ledCount = ledCounts[i];
		this->ledConfig[i].type = ANIMATOR_DEFAULT_TYPE;
		this->ledConfig[i].speed = ANIMATOR_DEFAULT_SPEED;
		this->ledConfig[i].offset = ANIMATOR_DEFAULT_OFFSET;
		this->ledConfig[i].brightness = ANIMATOR_DEFAULT_BRIGHTNESS;
		this->ledConfig[i].reverse = ANIMATOR_DEFAULT_REVERSE;
		this->ledConfig[i].fadeSpeed = ANIMATOR_DEFAULT_FADE_SPEED;
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			this->ledConfig[i].animationSettings[j] = 0;
		}
		this->ledConfig[i].ledVoltage = REGULATOR_DEFAULT_VOLTAGE;
		this->ledConfig[i].ledChannelCurrent[0] = LED_DEFAULT_CHANNEL_CURRENT;
		this->ledConfig[i].ledChannelCurrent[1] = LED_DEFAULT_CHANNEL_CURRENT;
		this->ledConfig[i].ledChannelCurrent[2] = LED_DEFAULT_CHANNEL_CURRENT;
	}

	// WiFi config
	this->wifiConfig.accessPointSsid = F(AP_DEFAULT_SSID);
	this->wifiConfig.accessPointPassword = F(AP_DEDAULT_PASSWORD);
	this->wifiConfig.accessPointChannel = AP_DEFAULT_CHANNEL;
	this->wifiConfig.accessPointHidden = AP_DEFAULT_HIDDEN;
	this->wifiConfig.accessPointMaxConnections = AP_DEFAULT_MAX_CONN;
	this->wifiConfig.wifiSsid = F(WIFI_DEFAULT_SSID);
	this->wifiConfig.wifiPassword = F(WIFI_DEFAULT_PASSWORD);

	// Motion sensor calibration
	this->motionSensorCalibration.accXRaw = 0;
	this->motionSensorCalibration.accYRaw = 0;
	this->motionSensorCalibration.accZRaw = 0;
	this->motionSensorCalibration.gyroXRaw = 0;
	this->motionSensorCalibration.gyroYRaw = 0;
	this->motionSensorCalibration.gyroZRaw = 0;
	this->motionSensorCalibration.accXG = 0.0f;
	this->motionSensorCalibration.accYG = 0.0f;
	this->motionSensorCalibration.accZG = 0.0f;
	this->motionSensorCalibration.gyroXDeg = 0.0f;
	this->motionSensorCalibration.gyroYDeg = 0.0f;
	this->motionSensorCalibration.gyroZDeg = 0.0f;

	// UI configuration
	this->uiConfiguration.firmware = FW_VERSION;
	this->uiConfiguration.language = UI_DEFAULT_LANGUAGE;
	this->uiConfiguration.theme = UI_DEFAULT_THEME;
	this->uiConfiguration.expertMode = UI_DEFAULT_EXPERT;
}

/**
 * @brief Load the configuration from a binary file.
 * @param loadDefaultIfNotExist load default values when the file could not be found
 * @return true when successful
 * @return false when there was an error
 */
bool TL::Configuration::load()
{
	TL::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_READ))
	{
		TL::Logger::log(TL::Logger::ERROR, SOURCE_LOCATION, (String)F("Failed to load configuration file: ") + fileName);
		return false;
	}

	// Check the configuration file version
	uint16_t configFileVersion = 0;
	if (!file.read(configFileVersion) || configFileVersion != this->configurationVersion)
	{
		TL::Logger::log(TL::Logger::ERROR, SOURCE_LOCATION, F("Configuration file version does not match."));
		file.close();
		return false;
	}

	// System config
	file.read(this->systemConfig.logLevel);
	file.read(this->systemConfig.lightSensorMode);
	file.read(this->systemConfig.lightSensorThreshold);
	file.read(this->systemConfig.lightSensorMinAmbientBrightness);
	file.read(this->systemConfig.lightSensorMaxAmbientBrightness);
	file.read(this->systemConfig.lightSensorMinLedBrightness);
	file.read(this->systemConfig.lightSensorMaxLedBrightness);
	file.read(this->systemConfig.lightSensorDuration);
	file.read(this->systemConfig.regulatorPowerLimit);
	file.read(this->systemConfig.regulatorHighTemperature);
	file.read(this->systemConfig.regulatorCutoffTemperature);
	file.read(this->systemConfig.fanMinPwmValue);
	file.read(this->systemConfig.fanMaxPwmValue);
	file.read(this->systemConfig.fanMinTemperature);
	file.read(this->systemConfig.fanMaxTemperature);

	// LED config
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		file.read(this->ledConfig[i].ledPin);
		file.read(this->ledConfig[i].ledCount);
		file.read(this->ledConfig[i].type);
		file.read(this->ledConfig[i].speed);
		file.read(this->ledConfig[i].offset);
		file.read(this->ledConfig[i].brightness);
		file.read(this->ledConfig[i].reverse);
		file.read(this->ledConfig[i].fadeSpeed);
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			file.read(this->ledConfig[i].animationSettings[j]);
		}
		file.read(this->ledConfig[i].ledVoltage);
		file.read(this->ledConfig[i].ledChannelCurrent[0]);
		file.read(this->ledConfig[i].ledChannelCurrent[1]);
		file.read(this->ledConfig[i].ledChannelCurrent[2]);
	}

	// WiFi config
	file.readString(this->wifiConfig.accessPointSsid);
	file.readString(this->wifiConfig.accessPointPassword);
	file.read(this->wifiConfig.accessPointChannel);
	file.read(this->wifiConfig.accessPointHidden);
	file.read(this->wifiConfig.accessPointMaxConnections);
	file.readString(this->wifiConfig.wifiSsid);
	file.readString(this->wifiConfig.wifiPassword);

	// Motion sensor calibration
	file.read(this->motionSensorCalibration.accXRaw);
	file.read(this->motionSensorCalibration.accYRaw);
	file.read(this->motionSensorCalibration.accZRaw);
	file.read(this->motionSensorCalibration.gyroXRaw);
	file.read(this->motionSensorCalibration.gyroYRaw);
	file.read(this->motionSensorCalibration.gyroZRaw);
	file.read(this->motionSensorCalibration.accXG);
	file.read(this->motionSensorCalibration.accYG);
	file.read(this->motionSensorCalibration.accZG);
	file.read(this->motionSensorCalibration.gyroXDeg);
	file.read(this->motionSensorCalibration.gyroYDeg);
	file.read(this->motionSensorCalibration.gyroZDeg);

	// UI configuration
	file.readString(this->uiConfiguration.language);
	file.readString(this->uiConfiguration.theme);
	file.read(this->uiConfiguration.expertMode);

	// Check the hash
	uint16_t fileHash = 0;
	if (!file.read(fileHash) || fileHash != this->getSimpleHash())
	{
		TL::Logger::log(TL::Logger::ERROR, SOURCE_LOCATION, F("Configuration file hash does not match."));
		file.close();
		return false;
	}

	file.close();
	return true;
}

/**
 * @brief Save to configuration to a file.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::Configuration::save()
{
	TL::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_WRITE))
	{
		TL::Logger::log(TL::Logger::ERROR, SOURCE_LOCATION, F("Failed to open configuration file."));
		return false;
	}

	// Write the configuration file version
	file.write(this->configurationVersion);

	// System cofiguration
	file.write((uint8_t)this->systemConfig.logLevel);
	file.write((uint8_t)this->systemConfig.lightSensorMode);
	file.write(this->systemConfig.lightSensorThreshold);
	file.write(this->systemConfig.lightSensorMinAmbientBrightness);
	file.write(this->systemConfig.lightSensorMaxAmbientBrightness);
	file.write(this->systemConfig.lightSensorMinLedBrightness);
	file.write(this->systemConfig.lightSensorMaxLedBrightness);
	file.write(this->systemConfig.lightSensorDuration);
	file.write(this->systemConfig.regulatorPowerLimit);
	file.write(this->systemConfig.regulatorHighTemperature);
	file.write(this->systemConfig.regulatorCutoffTemperature);
	file.write(this->systemConfig.fanMinPwmValue);
	file.write(this->systemConfig.fanMaxPwmValue);
	file.write(this->systemConfig.fanMinTemperature);
	file.write(this->systemConfig.fanMaxTemperature);

	// LED configuration
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		file.write(this->ledConfig[i].ledPin);
		file.write(this->ledConfig[i].ledCount);
		file.write(this->ledConfig[i].type);
		file.write(this->ledConfig[i].speed);
		file.write(this->ledConfig[i].offset);
		file.write(this->ledConfig[i].brightness);
		file.write(this->ledConfig[i].reverse);
		file.write(this->ledConfig[i].fadeSpeed);
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			file.write(this->ledConfig[i].animationSettings[j]);
		}
		file.write(this->ledConfig[i].ledVoltage);
		file.write(this->ledConfig[i].ledChannelCurrent[0]);
		file.write(this->ledConfig[i].ledChannelCurrent[1]);
		file.write(this->ledConfig[i].ledChannelCurrent[2]);
	}

	// WiFi configuration
	file.writeString(this->wifiConfig.accessPointSsid);
	file.writeString(this->wifiConfig.accessPointPassword);
	file.write(this->wifiConfig.accessPointChannel);
	file.write(this->wifiConfig.accessPointHidden);
	file.write(this->wifiConfig.accessPointMaxConnections);
	file.writeString(this->wifiConfig.wifiSsid);
	file.writeString(this->wifiConfig.wifiPassword);

	// Motion sensor calibration
	file.write(this->motionSensorCalibration.accXRaw);
	file.write(this->motionSensorCalibration.accYRaw);
	file.write(this->motionSensorCalibration.accZRaw);
	file.write(this->motionSensorCalibration.gyroXRaw);
	file.write(this->motionSensorCalibration.gyroYRaw);
	file.write(this->motionSensorCalibration.gyroZRaw);
	file.write(this->motionSensorCalibration.accXG);
	file.write(this->motionSensorCalibration.accYG);
	file.write(this->motionSensorCalibration.accZG);
	file.write(this->motionSensorCalibration.gyroXDeg);
	file.write(this->motionSensorCalibration.gyroYDeg);
	file.write(this->motionSensorCalibration.gyroZDeg);

	// UI configuration
	file.writeString(this->uiConfiguration.language);
	file.writeString(this->uiConfiguration.theme);
	file.write(this->uiConfiguration.expertMode);

	// Write the hash
	file.write(this->getSimpleHash());

	file.close();
	return true;
}

/**
 * @brief Calculate a very simple hash to check the config file.
 * @return uint16_t 2 byte hash
 */
uint16_t TL::Configuration::getSimpleHash()
{
	uint16_t hash = 7;

	// Configuration file version
	hash = hash * 31 + this->configurationVersion;

	// System configuration
	hash = hash * 31 + this->systemConfig.logLevel;
	hash = hash * 31 + this->systemConfig.lightSensorMode;
	hash = hash * 31 + this->systemConfig.lightSensorThreshold;
	hash = hash * 31 + this->systemConfig.lightSensorMinAmbientBrightness;
	hash = hash * 31 + this->systemConfig.lightSensorMaxAmbientBrightness;
	hash = hash * 31 + this->systemConfig.lightSensorMinLedBrightness;
	hash = hash * 31 + this->systemConfig.lightSensorMaxLedBrightness;
	hash = hash * 31 + this->systemConfig.regulatorPowerLimit;
	hash = hash * 31 + this->systemConfig.regulatorHighTemperature;
	hash = hash * 31 + this->systemConfig.regulatorCutoffTemperature;
	hash = hash * 31 + this->systemConfig.fanMinPwmValue;
	hash = hash * 31 + this->systemConfig.fanMaxPwmValue;
	hash = hash * 31 + this->systemConfig.fanMinTemperature;
	hash = hash * 31 + this->systemConfig.fanMaxTemperature;

	// LED configuration
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		hash = hash * 31 + this->ledConfig[i].ledPin;
		hash = hash * 31 + this->ledConfig[i].ledCount;
		hash = hash * 31 + this->ledConfig[i].type;
		hash = hash * 31 + this->ledConfig[i].speed;
		hash = hash * 31 + this->ledConfig[i].offset;
		hash = hash * 31 + this->ledConfig[i].brightness;
		hash = hash * 31 + this->ledConfig[i].reverse;
		hash = hash * 31 + this->ledConfig[i].fadeSpeed;
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			hash = hash * 31 + this->ledConfig[i].animationSettings[j];
		}
		hash = hash * 31 + this->ledConfig[i].ledVoltage;
		hash = hash * 31 + this->ledConfig[i].ledChannelCurrent[0];
		hash = hash * 31 + this->ledConfig[i].ledChannelCurrent[1];
		hash = hash * 31 + this->ledConfig[i].ledChannelCurrent[2];
	}

	// WiFi configuration
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.accessPointSsid);
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.accessPointPassword);
	hash = hash * 31 + this->wifiConfig.accessPointChannel;
	hash = hash * 31 + this->wifiConfig.accessPointHidden;
	hash = hash * 31 + this->wifiConfig.accessPointMaxConnections;
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.wifiSsid);
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.wifiPassword);

	// Motion sensor calibration
	hash = hash * 31 + this->motionSensorCalibration.accXRaw;
	hash = hash * 31 + this->motionSensorCalibration.accYRaw;
	hash = hash * 31 + this->motionSensorCalibration.accZRaw;
	hash = hash * 31 + this->motionSensorCalibration.gyroXRaw;
	hash = hash * 31 + this->motionSensorCalibration.gyroYRaw;
	hash = hash * 31 + this->motionSensorCalibration.gyroZRaw;
	hash = hash * 31 + this->motionSensorCalibration.accXG;
	hash = hash * 31 + this->motionSensorCalibration.accYG;
	hash = hash * 31 + this->motionSensorCalibration.accZG;
	hash = hash * 31 + this->motionSensorCalibration.gyroXDeg;
	hash = hash * 31 + this->motionSensorCalibration.gyroYDeg;
	hash = hash * 31 + this->motionSensorCalibration.gyroZDeg;

	// UI configuration
	hash = hash * 32 + this->getSimpleStringHash(this->uiConfiguration.language);
	hash = hash * 32 + this->getSimpleStringHash(this->uiConfiguration.theme);
	hash = hash * 32 + this->uiConfiguration.expertMode;

	return hash;
}

/**
 * @brief Create a very simple hash based on a string.
 * @param input input string
 * @return uint16_t 2 byte hash
 */
uint16_t TL::Configuration::getSimpleStringHash(const String input)
{
	uint16_t hash = 7;
	for (uint16_t i = 0; i < input.length(); i++)
	{
		hash = hash * 31 + input.charAt(i);
	}
	return hash;
}