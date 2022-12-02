/**
 * @file Configuration.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TesLight::Configuration}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "configuration/Configuration.h"

/**
 * @brief Create a new instance of {@link TesLight::Configuration}.
 * @param fileSystem pointer to a {@link FS}
 * @param fileName file name of the configuration file
 */
TesLight::Configuration::Configuration(FS *fileSystem, const String fileName)
{
	this->fileSystem = fileSystem;
	this->fileName = fileName;
	this->configurationVersion = 6;
	this->loadDefaults();
}

/**
 * @brief Destroy the {@link TesLight::Configuration} instance.
 */
TesLight::Configuration::~Configuration()
{
}

/**
 * @brief Get the System config.
 * @return instance of {@link TesLight::Configuration::SystemConfig}
 */
TesLight::Configuration::SystemConfig TesLight::Configuration::getSystemConfig()
{
	return this->systemConfig;
}

/**
 * @brief Set the System config.
 * @param SystemConfig config to set
 */
void TesLight::Configuration::setSystemConfig(TesLight::Configuration::SystemConfig systemConfig)
{
	this->systemConfig = systemConfig;
}

/**
 * @brief Get the {@link TesLight::Configuration::LedConfig}.
 * @param index index of the config
 * @return instance of {TesLight::Configuration::LedConfig}
 */
TesLight::Configuration::LedConfig TesLight::Configuration::getLedConfig(const uint8_t index)
{
	return this->ledConfig[index];
}

/**
 * @brief Set the {@link TesLight::Configuration::LedConfig}.
 * @param ledConfig instance of {@link TesLight::Configuration::LedConfig}
 * @param index index of the config
 */
void TesLight::Configuration::setLedConfig(const TesLight::Configuration::LedConfig ledConfig, const uint8_t index)
{
	this->ledConfig[index] = ledConfig;
}

/**
 * @brief Get the WiFi config.
 * @return instance of {@link TesLight::Configuration::WiFiConfig}
 */
TesLight::Configuration::WiFiConfig TesLight::Configuration::getWiFiConfig()
{
	return this->wifiConfig;
}

/**
 * @brief Set the WiFi config.
 * @param wifionfig config to set
 */
void TesLight::Configuration::setWiFiConfig(TesLight::Configuration::WiFiConfig wifiConfig)
{
	this->wifiConfig = wifiConfig;
}

/**
 * @brief Load the default configuration.
 */
void TesLight::Configuration::loadDefaults()
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
		for (uint8_t j = 0; j < ANIMATOR_NUM_CUSTOM_FIELDS; j++)
		{
			this->ledConfig[i].customField[j] = 0;
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
}

/**
 * @brief Load the configuration from a binary file.
 * @param loadDefaultIfNotExist load default values when the file could not be found
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Configuration::load()
{
	TesLight::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_READ))
	{
		TesLight::Logger::log(TesLight::Logger::ERROR, SOURCE_LOCATION, (String)F("Failed to load configuration file: ") + fileName);
		return false;
	}

	// Check the configuration file version
	if (file.readWord() != this->configurationVersion)
	{
		TesLight::Logger::log(TesLight::Logger::ERROR, SOURCE_LOCATION, F("Configuration file version does not match."));
		file.close();
		return false;
	}

	// System config
	this->systemConfig.logLevel = file.readByte();
	this->systemConfig.lightSensorMode = file.readByte();
	this->systemConfig.lightSensorThreshold = file.readByte();
	this->systemConfig.lightSensorMinAmbientBrightness = file.readByte();
	this->systemConfig.lightSensorMaxAmbientBrightness = file.readByte();
	this->systemConfig.lightSensorMinLedBrightness = file.readByte();
	this->systemConfig.lightSensorMaxLedBrightness = file.readByte();
	this->systemConfig.lightSensorDuration = file.readByte();
	this->systemConfig.regulatorPowerLimit = file.readByte();
	this->systemConfig.regulatorHighTemperature = file.readByte();
	this->systemConfig.regulatorCutoffTemperature = file.readByte();
	this->systemConfig.fanMinPwmValue = file.readByte();
	this->systemConfig.fanMaxPwmValue = file.readByte();
	this->systemConfig.fanMinTemperature = file.readByte();
	this->systemConfig.fanMaxTemperature = file.readByte();

	// LED config
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		this->ledConfig[i].ledPin = file.readByte();
		this->ledConfig[i].ledCount = file.readWord();
		this->ledConfig[i].type = file.readByte();
		this->ledConfig[i].speed = file.readByte();
		this->ledConfig[i].offset = file.readWord();
		this->ledConfig[i].brightness = file.readByte();
		this->ledConfig[i].reverse = file.readByte();
		this->ledConfig[i].fadeSpeed = file.readByte();
		for (uint8_t j = 0; j < ANIMATOR_NUM_CUSTOM_FIELDS; j++)
		{
			this->ledConfig[i].customField[j] = file.readByte();
		}
		this->ledConfig[i].ledVoltage = file.readByte();
		this->ledConfig[i].ledChannelCurrent[0] = file.readByte();
		this->ledConfig[i].ledChannelCurrent[1] = file.readByte();
		this->ledConfig[i].ledChannelCurrent[2] = file.readByte();
	}

	// WiFi config
	this->wifiConfig.accessPointSsid = file.readString();
	this->wifiConfig.accessPointPassword = file.readString();
	this->wifiConfig.accessPointChannel = file.readByte();
	this->wifiConfig.accessPointHidden = file.readByte();
	this->wifiConfig.accessPointMaxConnections = file.readByte();
	this->wifiConfig.wifiSsid = file.readString();
	this->wifiConfig.wifiPassword = file.readString();

	// Check the hash
	if (file.readWord() != this->getSimpleHash())
	{
		TesLight::Logger::log(TesLight::Logger::ERROR, SOURCE_LOCATION, F("Configuration file hash does not match."));
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
bool TesLight::Configuration::save()
{
	TesLight::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_WRITE))
	{
		TesLight::Logger::log(TesLight::Logger::ERROR, SOURCE_LOCATION, F("Failed to open configuration file."));
		return false;
	}

	// Write the configuration file version
	file.writeWord(this->configurationVersion);

	// System cofiguration
	file.writeByte((uint8_t)this->systemConfig.logLevel);
	file.writeByte((uint8_t)this->systemConfig.lightSensorMode);
	file.writeByte(this->systemConfig.lightSensorThreshold);
	file.writeByte(this->systemConfig.lightSensorMinAmbientBrightness);
	file.writeByte(this->systemConfig.lightSensorMaxAmbientBrightness);
	file.writeByte(this->systemConfig.lightSensorMinLedBrightness);
	file.writeByte(this->systemConfig.lightSensorMaxLedBrightness);
	file.writeByte(this->systemConfig.lightSensorDuration);
	file.writeByte(this->systemConfig.regulatorPowerLimit);
	file.writeByte(this->systemConfig.regulatorHighTemperature);
	file.writeByte(this->systemConfig.regulatorCutoffTemperature);
	file.writeByte(this->systemConfig.fanMinPwmValue);
	file.writeByte(this->systemConfig.fanMaxPwmValue);
	file.writeByte(this->systemConfig.fanMinTemperature);
	file.writeByte(this->systemConfig.fanMaxTemperature);

	// LED configuration
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		file.writeByte(this->ledConfig[i].ledPin);
		file.writeWord(this->ledConfig[i].ledCount);
		file.writeByte(this->ledConfig[i].type);
		file.writeByte(this->ledConfig[i].speed);
		file.writeWord(this->ledConfig[i].offset);
		file.writeByte(this->ledConfig[i].brightness);
		file.writeByte(this->ledConfig[i].reverse);
		file.writeByte(this->ledConfig[i].fadeSpeed);
		for (uint8_t j = 0; j < ANIMATOR_NUM_CUSTOM_FIELDS; j++)
		{
			file.writeByte(this->ledConfig[i].customField[j]);
		}
		file.writeByte(this->ledConfig[i].ledVoltage);
		file.writeByte(this->ledConfig[i].ledChannelCurrent[0]);
		file.writeByte(this->ledConfig[i].ledChannelCurrent[1]);
		file.writeByte(this->ledConfig[i].ledChannelCurrent[2]);
	}

	// WiFi configuration
	file.writeString(this->wifiConfig.accessPointSsid);
	file.writeString(this->wifiConfig.accessPointPassword);
	file.writeByte(this->wifiConfig.accessPointChannel);
	file.writeByte(this->wifiConfig.accessPointHidden);
	file.writeByte(this->wifiConfig.accessPointMaxConnections);
	file.writeString(this->wifiConfig.wifiSsid);
	file.writeString(this->wifiConfig.wifiPassword);

	// Write the hash
	file.writeWord(this->getSimpleHash());

	file.close();
	return true;
}

/**
 * @brief Calculate a very simple hash to check the config file.
 * @return uint16_t 2 byte hash
 */
uint16_t TesLight::Configuration::getSimpleHash()
{
	uint16_t hash = 7;
	hash = hash * 31 + this->configurationVersion;
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
		for (uint8_t j = 0; j < ANIMATOR_NUM_CUSTOM_FIELDS; j++)
		{
			hash = hash * 31 + this->ledConfig[i].customField[j];
		}
		hash = hash * 31 + this->ledConfig[i].ledVoltage;
		hash = hash * 31 + this->ledConfig[i].ledChannelCurrent[0];
		hash = hash * 31 + this->ledConfig[i].ledChannelCurrent[1];
		hash = hash * 31 + this->ledConfig[i].ledChannelCurrent[2];
	}
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.accessPointSsid);
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.accessPointPassword);
	hash = hash * 31 + this->wifiConfig.accessPointChannel;
	hash = hash * 31 + this->wifiConfig.accessPointHidden;
	hash = hash * 31 + this->wifiConfig.accessPointMaxConnections;
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.wifiSsid);
	hash = hash * 31 + this->getSimpleStringHash(this->wifiConfig.wifiPassword);
	return hash;
}

/**
 * @brief Create a very simple hash based on a string.
 * @param input input string
 * @return uint16_t 2 byte hash
 */
uint16_t TesLight::Configuration::getSimpleStringHash(const String input)
{
	uint16_t hash = 7;
	for (uint16_t i = 0; i < input.length(); i++)
	{
		hash = hash * 31 + input.charAt(i);
	}
	return hash;
}