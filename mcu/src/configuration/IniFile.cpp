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
	File file = this->fileSystem->open(fileName, FILE_READ);
	if (!file || file.isDirectory())
	{
		return false;
	}

	this->keyValueMap.clear();
	while (file.available())
	{
		const String fileLine = this->readNextLine(&file);
		if (!this->isValidLine(fileLine))
		{
			return false;
		}

		this->keyValueMap.insert(std::make_pair(this->extractKey(fileLine), this->extractValue(fileLine)));
	}

	file.close();
	return this->isValid();
}

/**
 * @brief Save the key-value pairs to an ini file.
 * @param fileName full path to the ini file
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::IniFile::save(const String fileName)
{
	File file = this->fileSystem->open(fileName, FILE_WRITE);
	if (!file)
	{
		return false;
	}

	for (auto const &pair : this->keyValueMap)
	{
		this->writeLine(&file, pair.first + ":" + pair.second);
	}

	file.close();
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
	if (this->keyExists(key))
	{
		return keyValueMap[key];
	}
	else
	{
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
	if (keyExists(key))
	{
		keyValueMap[key] = value;
	}
	else
	{
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
	if (!this->keyExists(key))
	{
		return false;
	}

	this->keyValueMap.erase(key);
	return true;
}

/**
 * @brief Check if the ini file is valid. To be valid every key must be unique.
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::IniFile::isValid()
{
	for (auto const &pair : this->keyValueMap)
	{
		if (this->keyValueMap.count(pair.first) != 1)
		{
			return false;
		}
	}

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
	if (iniFileLine.length() < 2)
	{
		return false;
	}

	return iniFileLine.indexOf(F(":")) > 0;
}

/**
 * @brief Extract the key from a single line of the ini file.
 * @param iniFileLine single line of the ini file
 * @return String the key parsed from the line
 */
String TesLight::IniFile::extractKey(const String iniFileLine)
{
	if (!this->isValidLine(iniFileLine))
	{
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
	if (!this->isValidLine(iniFileLine))
	{
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
