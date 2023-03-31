/**
 * @file Configuration.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TesLight::Configuration}.
 * @version 0.0.1
 * @date 2022-06-28
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
	this->apSSID = F("");
	this->apPassword = F("");
	this->apChannel = 0;
	this->apHidden = false;
	this->apMaxConnections = 0;
	this->wifiSSID = F("");
	this->wifiPassword = F("");
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		this->ledDriverConfig[i].ledPin = 0;
		this->ledDriverConfig[i].ledCount = 0;
		this->ledAnimatorConfig[i].type = 0;
		this->ledAnimatorConfig[i].speed = 0;
		this->ledAnimatorConfig[i].offset = 0;
		this->ledAnimatorConfig[i].reverse = false;
		for (uint8_t j = 0; j < 10; j++)
		{
			this->ledAnimatorConfig[i].customField[j] = 0;
		}
	}
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:Configuration"), F("Configuration initialized."));
}

/**
 * @brief Destroy the {@link TesLight::Configuration} instance.
 */
TesLight::Configuration::~Configuration()
{
}

/**
 * @brief Get the SSID for the access point.
 * @return String SSID for the ap
 */
String TesLight::Configuration::getApSSID()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getApSSID"), F("Get access point ssid."));
	return apSSID;
}

/**
 * @brief Set the SSID for the access point.
 * @param ssid SSID for the ap
 */
void TesLight::Configuration::setApSSID(const String ssid)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setApSSID"), (String)F("Set access point ssid to: ") + ssid);
	this->apSSID = ssid;
}

/**
 * @brief Get the channel of the access point.
 * @return uint8_t channel of the ap
 */
uint8_t TesLight::Configuration::getApChannel()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getApChannel"), F("Get access point channel."));
	return this->apChannel;
}

/**
 * @brief Set the channel of the access point.
 * @param apChannel channel of the ap
 */
void TesLight::Configuration::setApChannel(const uint8_t apChannel)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setApChannel"), (String)F("Set access point channel to: ") + String(apChannel));
	this->apChannel = apChannel;
}

/**
 * @brief Get if the access point is hidden or visible.
 * @return true when hidden
 * @return false when visible
 */
bool TesLight::Configuration::getApHidden()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getApHidden"), F("Get access point hidden."));
	return this->apHidden;
}

/**
 * @brief Set if the access point is hidden or visible.
 * @param apHidden {@code true} for hidden, {@code false} for visible
 */
void TesLight::Configuration::setApHidden(const bool apHidden)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setApHidden"), (String)F("Set access point hidden to: ") + String(apHidden));
	this->apHidden = apHidden;
}

/**
 * @brief Get the maximum number of connections to the access point.
 * @return uint8_t maximum number of connections to the ap
 */
uint8_t TesLight::Configuration::getApMaxConnections()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getApMaxConnections"), F("Get access point max connections."));
	return this->apMaxConnections;
}

/**
 * @brief Set the maximum number of connections to the access point.
 * @param apMaxConnections maximum number of connection to the ap
 */
void TesLight::Configuration::setApMaxConnections(const uint8_t apMaxConnections)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setApMaxConnections"), (String)F("Set access point max connections to: ") + String(apMaxConnections));
	this->apMaxConnections = apMaxConnections;
}

/**
 * @brief Get the password for the access point.
 * @return String password for the ap
 */
String TesLight::Configuration::getApPassword()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getApPassword"), F("Get access point password."));
	return apPassword;
}

/**
 * @brief Set the password for the access point.
 * @param password password for the ap
 */
void TesLight::Configuration::setApPassword(const String password)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setApPassword"), F("Set access point password to: <hidden>"));
	this->apPassword = password;
}

/**
 * @brief Get the SSID of the WiFi network.
 * @return String SSID of the WiFi network
 */
String TesLight::Configuration::getWifiSSID()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getWifiSSID"), F("Get WiFi network ssid."));
	return wifiSSID;
}

/**
 * @brief Set the SSID of the WiFi network.
 * @param ssid SSIF of the WiFi network
 */
void TesLight::Configuration::setWifiSSID(const String ssid)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setWifiSSID"), (String)F("Set WiFi network ssid to: ") + ssid);
	this->wifiSSID = ssid;
}

/**
 * @brief Get the password of the WiFi network.
 * @return String passworf of the WiFi network
 */
String TesLight::Configuration::getWifiPassword()
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getWifiPassword"), F("Get WiFi password."));
	return wifiPassword;
}

/**
 * @brief Set the password of the WiFi network.
 * @param password password of the WiFi network
 */
void TesLight::Configuration::setWifiPassword(const String password)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setWifiPassword"), F("Set WiFi password to: <hidden>"));
	this->wifiPassword = password;
}

/**
 * @brief Get the {@link TesLight::LedDriverConfig}.
 * @param index index of the config
 * @return instance of {TesLight::LedDriverConfig}
 */
TesLight::LedDriverConfig TesLight::Configuration::getLedDriverConfig(const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getLedDriverConfig"), (String)F("Get LedDriverConfig with index: ") + String(index));
	return this->ledDriverConfig[index];
}

/**
 * @brief Set the {@link TesLight::LedDriverConfig}.
 * @param ledDriverConfig instance of {@link TesLight::LedDriverConfig}
 * @param index index of the config
 */
void TesLight::Configuration::setLedDriverConfig(const TesLight::LedDriverConfig ledDriverConfig, const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setLedDriverConfig"), (String)F("Set LedDriverConfig with index: ") + String(index));
	this->ledDriverConfig[index] = ledDriverConfig;
}

/**
 * @brief Get the {@link TesLight::LedAnimatorConfig}.
 * @param index index of the config
 * @return instance of {TesLight::LedAnimatorConfig}
 */
TesLight::LedAnimatorConfig TesLight::Configuration::getLedAnimatorConfig(const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:getLedAnimatorConfig"), (String)F("Get LedAnimatorConfig with index: ") + String(index));
	return this->ledAnimatorConfig[index];
}

/**
 * @brief Set the {@link TesLight::LedAnimatorConfig}.
 * @param ledDriverConfig instance of {@link TesLight::LedAnimatorConfig}
 * @param index index of the config
 */
void TesLight::Configuration::setLedAnimatorConfig(const TesLight::LedAnimatorConfig ledAnimatorConfig, const uint8_t index)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:setLedAnimatorConfig"), (String)F("Set LedAnimatorConfig with index: ") + String(index));
	this->ledAnimatorConfig[index] = ledAnimatorConfig;
}

/**
 * @brief Load the configuration from file.
 * @param loadDefaultIfNotExist load default values when the file could not be opened
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Configuration::load(bool loadDefaultIfNotExist)
{
	TesLight::Logger::log(TesLight::Logger::INFO, F("Configuration.cpp:load"), (String)F("Loading configuration from file \"") + this->fileName + F("\"..."));
	TesLight::IniFile iniFile(this->fileSystem);
	if (!iniFile.load(this->fileName))
	{
		if (!loadDefaultIfNotExist)
		{
			TesLight::Logger::log(TesLight::Logger::ERROR, F("Configuration.cpp:load"), F("Failed to load configuration. The file could not be opened."));
			return false;
		}
	}
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:load"), F("Configuration file loaded."));

	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:load"), F("Parsing configuration from file..."));
	this->apSSID = iniFile.getValue(F("AP_SSID"), F("TesLight"));
	this->apPassword = iniFile.getValue(F("AP_PASSWORD"), F("TesLight"));
	this->apChannel = iniFile.getValue(F("AP_CHANNEL"), F("1")).toInt();
	this->apHidden = iniFile.getValue(F("AP_HIDDEN"), F("0")).toInt();
	this->apMaxConnections = iniFile.getValue(F("AP_MAX_CONNECTIONS"), F("1")).toInt();
	this->wifiSSID = iniFile.getValue(F("WIFI_SSID"));
	this->wifiPassword = iniFile.getValue(F("WIFI_PASSWORD"));

	const int *ledDriverConfig = this->parseIntArray(iniFile.getValue(F("LED_DRIVERS"), F("13,70, 14,4, 15,2, 16,4, 17,4, 21,4")), 2 * NUM_LED_DRIVERS);
	const int *ledAnimatorConfig = this->parseIntArray(iniFile.getValue(F("LED_ANIMATORS"), F("0,50,10,25,0,0,0,0,0,0,0,0,0,0,0, 0,50,10,25,0,0,0,0,0,0,0,0,0,0,0, 0,50,10,25,0,0,0,0,0,0,0,0,0,0,0, 0,50,10,25,0,0,0,0,0,0,0,0,0,0,0, 0,50,10,25,0,0,0,0,0,0,0,0,0,0,0, 0,50,10,25,0,0,0,0,0,0,0,0,0,0,0")), 15 * NUM_LED_DRIVERS);
	uint16_t driverConfigIndex = 0;
	uint16_t animatorConfigIndex = 0;
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		this->ledDriverConfig[i].ledPin = ledDriverConfig[driverConfigIndex++];
		this->ledDriverConfig[i].ledCount = ledDriverConfig[driverConfigIndex++];

		this->ledAnimatorConfig[i].type = ledAnimatorConfig[animatorConfigIndex++];
		this->ledAnimatorConfig[i].speed = ledAnimatorConfig[animatorConfigIndex++];
		this->ledAnimatorConfig[i].offset = ledAnimatorConfig[animatorConfigIndex++];
		this->ledAnimatorConfig[i].brightness = ledAnimatorConfig[animatorConfigIndex++];
		this->ledAnimatorConfig[i].reverse = ledAnimatorConfig[animatorConfigIndex++];
		for (uint8_t j = 0; j < 10; j++)
		{
			this->ledAnimatorConfig[i].customField[j] = ledAnimatorConfig[animatorConfigIndex++];
		}
	}
	delete[] ledAnimatorConfig;
	delete[] ledDriverConfig;
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
	TesLight::IniFile iniFile(this->fileSystem);
	iniFile.setValue(F("AP_SSID"), this->apSSID);
	iniFile.setValue(F("AP_PASSWORD"), this->apPassword);
	iniFile.setValue(F("WIFI_SSID"), this->wifiSSID);
	iniFile.setValue(F("WIFI_PASSWORD"), this->wifiPassword);

	int *ledDriverConfig = new int[2 * NUM_LED_DRIVERS];
	int *ledAnimatorConfig = new int[14 * NUM_LED_DRIVERS];
	uint16_t driverConfigIndex = 0;
	uint16_t animatorConfigIndex = 0;
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		ledDriverConfig[driverConfigIndex++] = this->ledDriverConfig[i].ledPin;
		ledDriverConfig[driverConfigIndex++] = this->ledDriverConfig[i].ledCount;

		ledAnimatorConfig[animatorConfigIndex++] = this->ledAnimatorConfig[i].type;
		ledAnimatorConfig[animatorConfigIndex++] = this->ledAnimatorConfig[i].speed;
		ledAnimatorConfig[animatorConfigIndex++] = this->ledAnimatorConfig[i].offset;
		ledAnimatorConfig[animatorConfigIndex++] = this->ledAnimatorConfig[i].brightness;
		ledAnimatorConfig[animatorConfigIndex++] = this->ledAnimatorConfig[i].reverse;
		for (uint8_t j = 0; j < 10; j++)
		{
			ledAnimatorConfig[animatorConfigIndex++] = this->ledAnimatorConfig[i].customField[j];
		}
	}
	iniFile.setValue(F("LED_DRIVERS"), this->intArrayToString(ledDriverConfig, 2 * NUM_LED_DRIVERS));
	iniFile.setValue(F("LED_ANIMATORS"), this->intArrayToString(ledAnimatorConfig, 15 * NUM_LED_DRIVERS));
	delete[] ledAnimatorConfig;
	delete[] ledDriverConfig;

	if (!iniFile.save(this->fileName))
	{
		TesLight::Logger::log(TesLight::Logger::ERROR, F("Configuration.cpp:save"), F("Failed to save configuration."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::INFO, F("Configuration.cpp:save"), F("Configuration saved."));
	return true;
}

/**
 * @brief Parse an int array from a {@link String}.
 * @param array {@link String} representation of the array
 * @param len number of elements
 * @return int* parsed array
 */
int *TesLight::Configuration::parseIntArray(const String array, const uint8_t len)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:parseIntArray"), (String)F("Parsing int array: ") + array);
	int *parsedArray = new int[len];
	uint8_t arrayIndex = 0;
	String buffer = F("");

	for (uint16_t i = 0; i < array.length(); i++)
	{
		if (array.charAt(i) >= '0' && array.charAt(i) <= '9')
		{
			buffer += array.charAt(i);
		}

		if (array.charAt(i) == ',' || i == array.length() - 1)
		{
			parsedArray[arrayIndex++] = buffer.toInt();
			buffer = F("");
		}
	}
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:parseIntArray"), F("Array parsed successfully."));

	return parsedArray;
}

/**
 * @brief Write an int array to a {@link String}.
 * @param array int array
 * @param len number of elements
 * @return {@link String} representation of the array
 */
String TesLight::Configuration::intArrayToString(const int *array, const uint8_t len)
{
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:intArrayToString"), F("Writing int array."));
	String buffer = F("");
	for (uint8_t i = 0; i < len; i++)
	{
		if (!buffer.isEmpty())
		{
			buffer += F(",");
		}
		buffer += array[i];
	}
	TesLight::Logger::log(TesLight::Logger::DEBUG, F("Configuration.cpp:intArrayToString"), F("Array written successfully."));
	return buffer;
}