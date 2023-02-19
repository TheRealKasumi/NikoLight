/**
 * @file Logger.cpp
 * @author TheRealKasumi
 * @brief Implementation of {@link TL::Logger}.
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
#include "logging/Logger.h"

bool TL::Logger::initialized = false;
bool TL::Logger::logToSerial;
bool TL::Logger::logToFile;
FS *TL::Logger::fileSystem;
String TL::Logger::fileName;
TL::Logger::LogLevel TL::Logger::minLogLevel;

/**
 * @brief Initialiize the {@link TL::Logger}.
 * @param minLogLevel optional parameter to set the minimum log level
 * @return true always
 */
bool TL::Logger::begin(const TL::Logger::LogLevel minLogLevel)
{
	TL::Logger::initialized = true;
	TL::Logger::logToSerial = false;
	TL::Logger::logToFile = false;
	TL::Logger::fileSystem = nullptr;
	TL::Logger::fileName = String();
	TL::Logger::minLogLevel = minLogLevel;
	return true;
}

/**
 * @brief Initialiize the {@link TL::Logger}.
 * @param baudRate serial baud rate
 * @param minLogLevel optional parameter to set the minimum log level
 * @return true always
 */
bool TL::Logger::begin(const uint32_t baudRate, const TL::Logger::LogLevel minLogLevel)
{
	Serial.begin(baudRate);
	TL::Logger::initialized = true;
	TL::Logger::logToSerial = true;
	TL::Logger::logToFile = false;
	TL::Logger::fileSystem = nullptr;
	TL::Logger::fileName = String();
	TL::Logger::minLogLevel = minLogLevel;
	return true;
}

/**
 * @brief Initialiize the {@link TL::Logger}.
 * @param fs instance of the {@link FS} containing the file
 * @param fn full name of the file
 * @param minLogLevel optional parameter to set the minimum log level
 * @return true when the logger can write to the sd card
 * @return false when the logger cant log to the sd card
 */
bool TL::Logger::begin(FS *fs, const String fn, const TL::Logger::LogLevel minLogLevel)
{
	TL::Logger::initialized = testOpenFile(fs, fn);
	TL::Logger::logToSerial = false;
	TL::Logger::logToFile = true;
	TL::Logger::fileSystem = fs;
	TL::Logger::fileName = fn;
	TL::Logger::minLogLevel = minLogLevel;
	return TL::Logger::initialized;
}

/**
 * @brief Initialiize the {@link TL::Logger}.
 * @param baudRate serial baud rate
 * @param fs instance of the {@link FS} containing the file
 * @param fn full name of the file
 * @param minLogLevel optional parameter to set the minimum log level
 * @return true when the logger can write to the sd card
 * @return false when the logger cant log to the sd card
 */
bool TL::Logger::begin(uint32_t baudRate, FS *fs, const String fn, const TL::Logger::LogLevel minLogLevel)
{
	Serial.begin(baudRate);
	TL::Logger::initialized = testOpenFile(fs, fn);
	TL::Logger::logToSerial = true;
	TL::Logger::logToFile = true;
	TL::Logger::fileSystem = fs;
	TL::Logger::fileName = fn;
	TL::Logger::minLogLevel = minLogLevel;
	return TL::Logger::initialized;
}

/**
 * @brief Stop the logger.
 */
void TL::Logger::end()
{
	TL::Logger::initialized = false;
	TL::Logger::logToSerial = false;
	TL::Logger::logToFile = false;
	TL::Logger::fileSystem = nullptr;
	TL::Logger::fileName = String();
	TL::Logger::minLogLevel = TL::Logger::LogLevel::INFO;
}

/**
 * @brief Check if the logger is initalized.
 * @return true when the logger is initalized
 * @return false when the logger is not initialized
 */
bool TL::Logger::isInitialized()
{
	return TL::Logger::initialized;
}

/**
 * @brief Set the minimum {@link TesLight.:LogLevel} that is logged.
 * @param logLevel minimum {@link TL::Logger::LogLevel}
 */
void TL::Logger::setMinLogLevel(const TL::Logger::LogLevel logLevel)
{
	minLogLevel = logLevel;
}

/**
 * @brief Log a message depending on the log level, source and message.
 * @param logLevel log level for the message
 * @param file path and name of the source file
 * @param function name of the function
 * @param current line in code
 * @param message message text
 */
void TL::Logger::log(const TL::Logger::LogLevel logLevel, const char *file, const char *function, const int line, const String message)
{
	if (!TL::Logger::initialized)
	{
		return;
	}

	if (logLevel < minLogLevel)
	{
		return;
	}

	const String logString = getTimeString() + F(" [") + getLogLevelString(logLevel) + F("] (") + String(file) + F(") (") + String(function) + F(") (") + String(line) + F("): ") + message + F("\r\n");

	if (logToSerial)
	{
		Serial.print(logString);
	}

	if (logToFile)
	{

		File logFile = fileSystem->open(fileName, FILE_APPEND);
		if (!logFile || logFile.isDirectory())
		{
			return;
		}
		logFile.write((uint8_t *)logString.c_str(), logString.length());
		logFile.close();
	}
}

/**
 * @brief Get the size of the log file on the SD card.
 * @return size_t log file size in bytes
 */
size_t TL::Logger::getLogSize()
{
	if (!TL::Logger::initialized)
	{
		return 0;
	}

	if (!logToFile)
	{
		return 0;
	}

	File logFile = fileSystem->open(fileName, FILE_READ);
	if (!logFile)
	{
		return 0;
	}

	const size_t logSize = logFile.size();
	logFile.close();
	return logSize;
}

/**
 * @brief Read a part from the log file stored on the SD card.
 * @param buffer buffer to storage for the data
 * @param start start index from where to read the log
 * @param bufferSize size of the buffer
 */
void TL::Logger::readLog(uint8_t *buffer, const size_t start, const size_t bufferSize)
{
	if (!TL::Logger::initialized)
	{
		return;
	}

	if (!logToFile)
	{
		return;
	}

	File logFile = fileSystem->open(fileName, FILE_READ);
	if (!logFile)
	{
		return;
	}

	logFile.seek(start);
	logFile.read(buffer, bufferSize);
	logFile.close();
}

/**
 * @brief Clear the log file on the SD card.
 */
void TL::Logger::clearLog()
{
	if (!TL::Logger::initialized)
	{
		return;
	}

	if (!logToFile)
	{
		return;
	}

	fileSystem->remove(fileName);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Log file was cleared."));
}

/**
 * @brief Test if a file can be opened.
 * @return true when file can be opened
 * @return false when file can not be opened
 */
bool TL::Logger::testOpenFile(FS *fs, const String fn)
{
	if (fs != nullptr)
	{
		File logFile = fs->open(fn, FILE_APPEND);
		if (logFile)
		{
			logFile.close();
			return true;
		}
	}

	return false;
}

/**
 * @brief Get the {@link String} representation of the {@link TL::Logger::LogLevel}.
 * @param logLevel log level
 * @return {@link String} representation of the {@link TL::Logger::LogLevel}
 */
String TL::Logger::getLogLevelString(const TL::Logger::LogLevel logLevel)
{
	if (logLevel == TL::Logger::LogLevel::DEBUG)
	{
		return F("DEBUG");
	}
	else if (logLevel == TL::Logger::LogLevel::INFO)
	{
		return F("INFO");
	}
	else if (logLevel == TL::Logger::LogLevel::WARN)
	{
		return F("WARN");
	}
	else if (logLevel == TL::Logger::LogLevel::ERROR)
	{
		return F("ERROR");
	}

	return F("UNKNOWN");
}

/**
 * @brief Get a formatted time {@link String} based on the system time.
 * @return {@link String} representation of the system time
 */
String TL::Logger::getTimeString()
{
	unsigned long milli = millis();
	const unsigned long hour = milli / 3600000;
	milli = milli - 3600000 * hour;
	const unsigned long min = milli / 60000;
	milli = milli - 60000 * min;
	const unsigned long sec = milli / 1000;
	milli = milli - 1000 * sec;

	String timeString = String();
	if (hour < 10)
	{
		timeString = timeString + F("0") + hour + F(":");
	}
	else
	{
		timeString = timeString + hour + F(":");
	}
	if (min < 10)
	{
		timeString = timeString + F("0") + min + F(":");
	}
	else
	{
		timeString = timeString + min + F(":");
	}
	if (sec < 10)
	{
		timeString = timeString + F("0") + sec + F(":");
	}
	else
	{
		timeString = timeString + sec + F(":");
	}
	if (milli < 10)
	{
		timeString = timeString + F("00") + milli;
	}
	else if (milli < 100)
	{
		timeString = timeString = timeString + F("0") + milli;
	}
	else
	{
		timeString = timeString + milli;
	}

	return timeString;
}