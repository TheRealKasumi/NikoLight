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
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:Configuration"), F("Initialize configuration..."));

	this->fileSystem = fileSystem;
	this->fileName = fileName;
	this->loadDefaults();

	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:Configuration"), F("Configuration initialized."));
}

/**
 * @brief Destroy the {@link TesLight::Configuration} instance.
 */
TesLight::Configuration::~Configuration()
{
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
 * @brief Get the System config.
 * @return instance of {@link TesLight::Configuration::SystemConfig}
 */
TesLight::Configuration::SystemConfig TesLight::Configuration::getSystemConfig()
{
	return this->systemConfig;
}

/**
 * @brief Get the {@link TesLight::Configuration::LedConfig}.
 * @param index index of the config
 * @return instance of {TesLight::Configuration::LedConfig}
 */
TesLight::Configuration::LedConfig TesLight::Configuration::getLedConfig(const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getLedConfig"), (String)F("Get LED configuration with index: ") + String(index));
	return this->ledConfig[index];
}

/**
 * @brief Set the {@link TesLight::Configuration::LedConfig}.
 * @param ledConfig instance of {@link TesLight::Configuration::LedConfig}
 * @param index index of the config
 */
void TesLight::Configuration::setLedConfig(const TesLight::Configuration::LedConfig ledConfig, const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setLedConfig"), (String)F("Set LED configuration with index: ") + String(index));
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
 * @brief Set the System config.
 * @param SystemConfig config to set
 */
void TesLight::Configuration::setSystemConfig(TesLight::Configuration::SystemConfig systemConfig)
{
	this->systemConfig = systemConfig;
}

/**
 * @brief Load the default configuration.
 */
void TesLight::Configuration::loadDefaults()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:loadDefaults"), F("Loading default configuration."));

	// System config
	this->systemConfig.logLevel = TesLight::Logger::LogLevel::INFO;
	this->systemConfig.lightSensorMode = TesLight::LightSensor::LightSensorMode::ALWAYS_ON;
	this->systemConfig.lightSensorThreshold = 30;
	this->systemConfig.lightSensorMinValue = 30;
	this->systemConfig.lightSensorMaxValue = 2048;

	// LED config
	const uint8_t ledPins[NUM_LED_DRIVERS] = {13, 14, 15, 16, 17, 21};
	const uint8_t ledCounts[NUM_LED_DRIVERS] = {70, 2, 4, 4, 4, 4};
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		this->ledConfig[i].ledPin = ledPins[i];
		this->ledConfig[i].ledCount = ledCounts[i];
		this->ledConfig[i].type = 0;
		this->ledConfig[i].speed = 50;
		this->ledConfig[i].offset = 10;
		this->ledConfig[i].brightness = 25;
		this->ledConfig[i].reverse = false;
		this->ledConfig[i].fadeSpeed = 5;
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			this->ledConfig[i].customField[j] = 0;
		}
	}

	// WiFi config
	this->wifiConfig.accessPointSsid = F("TesLight");
	this->wifiConfig.accessPointPassword = F("TesLight");
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
	TesLight::Logger::log(TesLight::Logger::INFO, F("Configuration.cpp:load"), (String)F("Loading configuration from file \"") + this->fileName + F("\"..."));

	TesLight::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_READ))
	{
		TesLight::Logger::log(TesLight::Logger::WARN, F("Configuration.cpp:load"), F("Failed to load configuration file."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:load"), F("Configuration file loaded. Parsing configuration from file..."));

	// System config
	this->systemConfig.logLevel = (TesLight::Logger::LogLevel)file.readByte();
	this->systemConfig.lightSensorMode = (TesLight::LightSensor::LightSensorMode)file.readByte();
	this->systemConfig.lightSensorThreshold = file.readWord();
	this->systemConfig.lightSensorMinValue = file.readWord();
	this->systemConfig.lightSensorMaxValue = file.readWord();

	// LED config
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
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

	file.close();

	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:load"), F("Configuration parsed."));
	return true;
}

/**
 * @brief Save to configuration to a file.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Configuration::save()
{
	TesLight::Logger::log(TesLight::Logger::INFO, F("Configuration.cpp:save"), (String)F("Saving configuration to file \"") + this->fileName + F("\"..."));

	TesLight::BinaryFile file(this->fileSystem);
	if (!file.open(this->fileName, FILE_WRITE))
	{
		TesLight::Logger::log(TesLight::Logger::WARN, F("Configuration.cpp:save"), F("Failed to open configuration file."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:save"), F("Configuration file opened. Writing configuration to file..."));

	// System cofiguration
	file.writeByte(this->systemConfig.logLevel);
	file.writeByte(this->systemConfig.lightSensorMode);
	file.writeWord(this->systemConfig.lightSensorThreshold);
	file.writeWord(this->systemConfig.lightSensorMinValue);
	file.writeWord(this->systemConfig.lightSensorMaxValue);

	// LED configuration
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
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

	file.close();

	TesLight::Logger::log(TesLight::Logger::INFO, F("Configuration.cpp:save"), F("Configuration saved."));
	return true;
}
