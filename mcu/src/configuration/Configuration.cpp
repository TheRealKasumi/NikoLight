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
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Initialize configuration."));

	this->fileSystem = fileSystem;
	this->fileName = fileName;
	this->configurationVersion = 1;
	this->loadDefaults();

	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Configuration initialized."));
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
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Get system configuration."));
	return this->systemConfig;
}

/**
 * @brief Set the System config.
 * @param SystemConfig config to set
 */
void TesLight::Configuration::setSystemConfig(TesLight::Configuration::SystemConfig systemConfig)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Set system configuration."));
	this->systemConfig = systemConfig;
}

/**
 * @brief Get the {@link TesLight::Configuration::LedConfig}.
 * @param index index of the config
 * @return instance of {TesLight::Configuration::LedConfig}
 */
TesLight::Configuration::LedConfig TesLight::Configuration::getLedConfig(const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, (String)F("Get LED configuration with index: ") + String(index));
	return this->ledConfig[index];
}

/**
 * @brief Set the {@link TesLight::Configuration::LedConfig}.
 * @param ledConfig instance of {@link TesLight::Configuration::LedConfig}
 * @param index index of the config
 */
void TesLight::Configuration::setLedConfig(const TesLight::Configuration::LedConfig ledConfig, const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, (String)F("Set LED configuration with index: ") + String(index));
	this->ledConfig[index] = ledConfig;
}

/**
 * @brief Get the WiFi config.
 * @return instance of {@link TesLight::Configuration::WiFiConfig}
 */
TesLight::Configuration::WiFiConfig TesLight::Configuration::getWiFiConfig()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Get WiFi configuration."));
	return this->wifiConfig;
}

/**
 * @brief Set the WiFi config.
 * @param wifionfig config to set
 */
void TesLight::Configuration::setWiFiConfig(TesLight::Configuration::WiFiConfig wifiConfig)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Set WiFi configuration."));
	this->wifiConfig = wifiConfig;
}

/**
 * @brief Load the default configuration.
 */
void TesLight::Configuration::loadDefaults()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Loading default configuration."));

	// System config
	this->systemConfig.logLevel = TesLight::Logger::LogLevel::INFO;
	this->systemConfig.lightSensorMode = TesLight::LightSensor::LightSensorMode::ALWAYS_ON;
	this->systemConfig.lightSensorThreshold = 30;  // Analog value
	this->systemConfig.lightSensorMinValue = 30;   // Analog value
	this->systemConfig.lightSensorMaxValue = 2048; // Analog value
	this->systemConfig.systemPowerLimit = 10;	   // W
	this->systemConfig.ledVoltage = 50;			   // Voltage * 10
	this->systemConfig.ledChannelCurrent[0] = 12;  // mA
	this->systemConfig.ledChannelCurrent[1] = 12;  // mA
	this->systemConfig.ledChannelCurrent[2] = 12;  // mA

	// LED config
	const uint8_t ledPins[NUM_LED_STRIPS] = {13, 14, 15, 16, 17, 21};
	const uint8_t ledCounts[NUM_LED_STRIPS] = {70, 2, 4, 4, 4, 4};
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		this->ledConfig[i].ledPin = ledPins[i];
		this->ledConfig[i].ledCount = ledCounts[i];
		this->ledConfig[i].type = 0;
		this->ledConfig[i].speed = 50;
		this->ledConfig[i].offset = 10;
		this->ledConfig[i].brightness = 50;
		this->ledConfig[i].reverse = false;
		this->ledConfig[i].fadeSpeed = 5;
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			this->ledConfig[i].customField[j] = 0;
		}
	}

	// WiFi config
	this->wifiConfig.accessPointSsid = F("TesLight");
	this->wifiConfig.accessPointPassword = F("TesLightPW");
	this->wifiConfig.accessPointChannel = 1;
	this->wifiConfig.accessPointHidden = false;
	this->wifiConfig.accessPointMaxConnections = 1;
	this->wifiConfig.wifiSsid = F("");
	this->wifiConfig.wifiPassword = F("");
}

/**
 * @brief Load the configuration from a binary file.
 * @param loadDefaultIfNotExist load default values when the file could not be found
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Configuration::load()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, (String)F("Loading configuration from file \"") + this->fileName + F("\"."));

	TesLight::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_READ))
	{
		TesLight::Logger::log(TesLight::Logger::ERROR, SOURCE_LOCATION, (String)F("Failed to load configuration file: ") + fileName);
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Configuration file loaded. Parsing configuration from file."));

	// Check the configuration file version
	if (file.readWord() != this->configurationVersion)
	{
		TesLight::Logger::log(TesLight::Logger::ERROR, SOURCE_LOCATION, F("Configuration file version does not match."));
		file.close();
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Reading binary data."));

	// System config
	this->systemConfig.logLevel = (TesLight::Logger::LogLevel)file.readByte();
	this->systemConfig.lightSensorMode = (TesLight::LightSensor::LightSensorMode)file.readByte();
	this->systemConfig.lightSensorThreshold = file.readWord();
	this->systemConfig.lightSensorMinValue = file.readWord();
	this->systemConfig.lightSensorMaxValue = file.readWord();
	this->systemConfig.systemPowerLimit = file.readByte();
	this->systemConfig.ledVoltage = file.readByte();
	this->systemConfig.ledChannelCurrent[0] = file.readByte();
	this->systemConfig.ledChannelCurrent[1] = file.readByte();
	this->systemConfig.ledChannelCurrent[2] = file.readByte();

	// LED config
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		this->ledConfig[i].ledPin = file.readByte();
		this->ledConfig[i].ledCount = file.readWord();
		this->ledConfig[i].type = file.readByte();
		this->ledConfig[i].speed = file.readByte();
		this->ledConfig[i].offset = file.readWord();
		this->ledConfig[i].brightness = file.readByte();
		this->ledConfig[i].reverse = file.readByte();
		this->ledConfig[i].fadeSpeed = file.readByte();
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			this->ledConfig[i].customField[j] = file.readByte();
		}
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

	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Configuration parsed."));
	return true;
}

/**
 * @brief Save to configuration to a file.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Configuration::save()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, (String)F("Saving configuration to file \"") + this->fileName + F("\"."));

	TesLight::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_WRITE))
	{
		TesLight::Logger::log(TesLight::Logger::WARN, SOURCE_LOCATION, F("Failed to open configuration file."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Configuration file opened. Writing configuration to file."));

	// Write the configuration file version
	file.writeWord(this->configurationVersion);

	// System cofiguration
	file.writeByte((uint8_t)this->systemConfig.logLevel);
	file.writeByte((uint8_t)this->systemConfig.lightSensorMode);
	file.writeWord(this->systemConfig.lightSensorThreshold);
	file.writeWord(this->systemConfig.lightSensorMinValue);
	file.writeWord(this->systemConfig.lightSensorMaxValue);
	file.writeByte(this->systemConfig.systemPowerLimit);
	file.writeByte(this->systemConfig.ledVoltage);
	file.writeByte(this->systemConfig.ledChannelCurrent[0]);
	file.writeByte(this->systemConfig.ledChannelCurrent[1]);
	file.writeByte(this->systemConfig.ledChannelCurrent[2]);

	// LED configuration
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		file.writeByte(this->ledConfig[i].ledPin);
		file.writeWord(this->ledConfig[i].ledCount);
		file.writeByte(this->ledConfig[i].type);
		file.writeByte(this->ledConfig[i].speed);
		file.writeWord(this->ledConfig[i].offset);
		file.writeByte(this->ledConfig[i].brightness);
		file.writeByte(this->ledConfig[i].reverse);
		file.writeByte(this->ledConfig[i].fadeSpeed);
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			file.writeByte(this->ledConfig[i].customField[j]);
		}
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

	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Configuration saved."));
	return true;
}

/**
 * @brief Calculate a very simple hash to check the config file.
 * @return uint16_t 2 byte hash
 */
uint16_t TesLight::Configuration::getSimpleHash()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Calculate hash of the configuration."));
	uint16_t hash = 7;
	hash = hash * 31 + this->configurationVersion;
	hash = hash * 31 + this->systemConfig.logLevel;
	hash = hash * 31 + this->systemConfig.lightSensorMode;
	hash = hash * 31 + this->systemConfig.lightSensorThreshold;
	hash = hash * 31 + this->systemConfig.lightSensorMinValue;
	hash = hash * 31 + this->systemConfig.lightSensorMaxValue;
	hash = hash * 31 + this->systemConfig.systemPowerLimit;
	hash = hash * 31 + this->systemConfig.ledVoltage;
	hash = hash * 31 + this->systemConfig.ledChannelCurrent[0];
	hash = hash * 31 + this->systemConfig.ledChannelCurrent[1];
	hash = hash * 31 + this->systemConfig.ledChannelCurrent[2];
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		hash = hash * 31 + this->ledConfig[i].ledPin;
		hash = hash * 31 + this->ledConfig[i].ledCount;
		hash = hash * 31 + this->ledConfig[i].type;
		hash = hash * 31 + this->ledConfig[i].speed;
		hash = hash * 31 + this->ledConfig[i].offset;
		hash = hash * 31 + this->ledConfig[i].brightness;
		hash = hash * 31 + this->ledConfig[i].reverse;
		hash = hash * 31 + this->ledConfig[i].fadeSpeed;
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			hash = hash * 31 + this->ledConfig[i].customField[j];
		}
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
	TesLight::Logger::log(TesLight::Logger::DEBUG, SOURCE_LOCATION, F("Calculate hash of the string."));
	uint16_t hash = 7;
	for (uint16_t i = 0; i < input.length(); i++)
	{
		hash = hash * 31 + input.charAt(i);
	}
	return hash;
}