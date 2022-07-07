/**
 * @file IniFile.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TesLight::IniFile}.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "configuration/IniFile.h"

/**
 * @brief Create a new instance of {@link TesLight::IniFile}.
 * @param fileSystem pointer to a working {@link FS}
 */
TesLight::IniFile::IniFile(FS *fileSystem)
{
	this->fileSystem = fileSystem;
}

/**
 * @brief Destroy the {@link TesLight::IniFile} instance.
 */
TesLight::IniFile::~IniFile()
{
}

/**
 * @brief Load key-value pairs from an ini file-
 * @param fileName full path to the ini file
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::IniFile::load(const String fileName)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("IniFile.cpp:load"), (String)F("Loading ini file: ") + fileName);
	File file = this->fileSystem->open(fileName, FILE_READ);
	if (!file || file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("IniFile.cpp:load"), F("Failed to open file for reading."));
		return false;
	}

	this->keyValueMap.clear();
	while (file.available())
	{
		const String fileLine = this->readNextLine(&file);
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:load"), (String)F("Ini file line: ") + fileLine);
		if (!this->isValidLine(fileLine))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("IniFile.cpp:load"), (String)F("The ini file is not valid. Error in line: ") + fileLine);
			return false;
		}

		this->keyValueMap.insert(std::make_pair(this->extractKey(fileLine), this->extractValue(fileLine)));
	}

	file.close();

	if (this->isValid())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("IniFile.cpp:load"), F("Ini file loaded successfully."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("IniFile.cpp:load"), F("Failed to parse ini file. The file is invalid."));
		return false;
	}
}

/**
 * @brief Save the key-value pairs to an ini file.
 * @param fileName full path to the ini file
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::IniFile::save(const String fileName)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("IniFile.cpp:save"), (String)F("Saving ini file: ") + fileName);
	File file = this->fileSystem->open(fileName, FILE_WRITE);
	if (!file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("IniFile.cpp:load"), F("Failed to open file for writing."));
		return false;
	}

	for (auto const &pair : this->keyValueMap)
	{
		const String line = pair.first + ":" + pair.second;
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:load"), (String)F("Writing line: ") + line);
		this->writeLine(&file, line);
	}

	file.close();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("IniFile.cpp:save"), F("Ini file saved successfully."));
	return true;
}

/**
 * @brief Checks if a key exists.
 * @param key name of the key
 * @return true when it exists
 * @return false when it not exists
 */
bool TesLight::IniFile::keyExists(const String key)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:keyExists"), (String)F("Checking if key \"") + key + F("\" exists in ini file."));
	return keyValueMap.count(key) > 0;
}

/**
 * @brief Get a value of a pair.
 * @param key name of the key
 * @param defaultValue default value in case the key was not found
 * @return String value
 */
String TesLight::IniFile::getValue(const String key, const String defaultValue)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:getValue"), (String)F("Reading value for key \"") + key + F("\"."));
	if (this->keyExists(key))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:keyExists"), (String)F("Key found, returning value \"") + keyValueMap[key] + F("\"."));
		return keyValueMap[key];
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:keyExists"), (String)F("Key not found, returning default value \"") + defaultValue + F("\"."));
		return defaultValue;
	}
}

/**
 * @brief Set the value for a pair.
 * @param key name of the key
 * @param value value for the key
 */
void TesLight::IniFile::setValue(const String key, const String value)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:setValue"), (String)F("Setting value for key \"") + key + F("\" to \"") + value + F("\"."));
	if (keyExists(key))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:setValue"), F("Key exists, overwriting value."));
		keyValueMap[key] = value;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:setValue"), F("Key does not exist. Adding new key-value pair."));
		keyValueMap.insert(std::make_pair(key, value));
	}
}

/**
 * @brief Remove a key-value pair.
 * @param key name of the key
 * @return true when the pair was removed
 * @return false when the pair was not found
 */
bool TesLight::IniFile::removeValue(const String key)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:removeValue"), (String)F("Removing value for key \"") + key + F("\"."));
	if (!this->keyExists(key))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:removeValue"), F("Key was not found, done."));
		return false;
	}

	this->keyValueMap.erase(key);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:removeValue"), F("Key-value pair was removed."));
	return true;
}

/**
 * @brief Check if the ini file is valid. To be valid every key must be unique.
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::IniFile::isValid()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:isValid"), F("Verifing ini file."));
	for (auto const &pair : this->keyValueMap)
	{
		if (this->keyValueMap.count(pair.first) != 1)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:isValid"), F("Verification failed. At least one key is duplicated."));
			return false;
		}
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:isValid"), F("The ini file is valid."));
	return true;
}

/**
 * @brief Check if a single line of the ini file is valid.
 * @param iniFileLine line to check
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::IniFile::isValidLine(const String iniFileLine)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:isValidLine"), (String)F("Verifing ini file line: ") + iniFileLine);
	if (iniFileLine.length() < 2)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:isValidLine"), F("Verification failed. The line is too short."));
		return false;
	}

	if (iniFileLine.indexOf(F(":")) > 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:isValidLine"), F("The line is valid."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:isValidLine"), F("Verification failed. The line is missing the \":\" separator."));
		return false;
	}
}

/**
 * @brief Extract the key from a single line of the ini file.
 * @param iniFileLine single line of the ini file
 * @return String the key parsed from the line
 */
String TesLight::IniFile::extractKey(const String iniFileLine)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:extractKey"), (String)F("Extracting key from line: ") + iniFileLine);
	if (!this->isValidLine(iniFileLine))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:extractKey"), F("The line is invalid."));
		return F("");
	}

	const int sepIndex = iniFileLine.indexOf(F(":"));
	return iniFileLine.substring(0, sepIndex);
}

/**
 * @brief Extract the value from a single line of the ini file.
 * @param iniFileLine single line of the ini file
 * @return String the value parsed from the line
 */
String TesLight::IniFile::extractValue(const String iniFileLine)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:extractKey"), (String)F("Extracting value from line: ") + iniFileLine);
	if (!this->isValidLine(iniFileLine))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("IniFile.cpp:extractValue"), F("The line is invalid."));
		return F("");
	}

	const int sepIndex = iniFileLine.indexOf(F(":"));
	return iniFileLine.substring(sepIndex + 1, iniFileLine.length());
}

/**
 * @brief Read the next line (terminated by '\n' or '\r\n' ) from a file.
 * @param file open file
 * @return String next line of the file
 */
String TesLight::IniFile::readNextLine(File *file)
{
	String line = F("");
	char buf = 0;
	while (file->available() && buf != '\n')
	{
		buf = (char)file->read();

		if (buf != '\r' && buf != '\n')
		{
			line += buf;
		}
	}
	return line;
}

/**
 * @brief Write a single line to a file.
 * @param file open file
 * @param input line to write
 */
void TesLight::IniFile::writeLine(File *file, const String input)
{
	const String line = input + F("\n");
	file->write((const uint8_t *)line.c_str(), line.length());
}
