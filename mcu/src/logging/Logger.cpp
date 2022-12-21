/**
 * @file Logger.cpp
 * @author TheRealKasumi
 * @brief Implementation of {@link TesLight::Logger}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "logging/Logger.h"

// Initialize
bool TesLight::Logger::logToSerial = false;
bool TesLight::Logger::logToFile = false;
FS *TesLight::Logger::fileSystem = nullptr;
String TesLight::Logger::fileName = F("");
TesLight::Logger::LogLevel TesLight::Logger::minLogLevel = TesLight::Logger::LogLevel::DEBUG;

/**
 * @brief Initialiize the {@link TesLight::Logger}.
 * @return true always
 */
bool TesLight::Logger::begin()
{
	return true;
}

/**
 * @brief Initialiize the {@link TesLight::Logger}.
 * @param baudRate serial baud rate
 * @return true always
 */
bool TesLight::Logger::begin(const uint32_t baudRate)
{
	Serial.begin(baudRate);
	logToSerial = true;
	return true;
}

/**
 * @brief Initialiize the {@link TesLight::Logger}.
 * @param fs instance of the {@link FS} containing the file
 * @param fn full name of the file
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Logger::begin(FS *fs, const String fn)
{
	logToFile = testOpenFile(fs, fn);
	fileSystem = fs;
	fileName = fn;
	return true;
}

/**
 * @brief Initialiize the {@link TesLight::Logger}.
 * @param baudRate serial baud rate
 * @param fs instance of the {@link FS} containing the file
 * @param fn full name of the file
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Logger::begin(uint32_t baudRate, FS *fs, const String fn)
{
	Serial.begin(baudRate);
	logToSerial = true;
	logToFile = testOpenFile(fs, fn);
	fileSystem = fs;
	fileName = fn;
	return true;
}

/**
 * @brief Set the minimum {@link TesLight.:LogLevel} that is logged.
 * @param logLevel minimum {@link TesLight::Logger::LogLevel}
 */
void TesLight::Logger::setMinLogLevel(const TesLight::Logger::LogLevel logLevel)
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
void TesLight::Logger::log(const TesLight::Logger::LogLevel logLevel, const char *file, const char *function, const int line, const String message)
{
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
size_t TesLight::Logger::getLogSize()
{
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
void TesLight::Logger::readLog(uint8_t *buffer, const size_t start, const size_t bufferSize)
{
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
void TesLight::Logger::clearLog()
{
	if (!logToFile)
	{
		return;
	}

	fileSystem->remove(fileName);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Log file was cleared."));
}

/**
 * @brief Test if a file can be opened.
 * @return true when file can be opened
 * @return false when file can not be opened
 */
bool TesLight::Logger::testOpenFile(FS *fs, const String fn)
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
 * @brief Get the {@link String} representation of the {@link TesLight::Logger::LogLevel}.
 * @param logLevel log level
 * @return {@link String} representation of the {@link TesLight::Logger::LogLevel}
 */
String TesLight::Logger::getLogLevelString(const TesLight::Logger::LogLevel logLevel)
{
	if (logLevel == TesLight::Logger::LogLevel::DEBUG)
	{
		return F("DEBUG");
	}
	else if (logLevel == TesLight::Logger::LogLevel::INFO)
	{
		return F("INFO");
	}
	else if (logLevel == TesLight::Logger::LogLevel::WARN)
	{
		return F("WARN");
	}
	else if (logLevel == TesLight::Logger::LogLevel::ERROR)
	{
		return F("ERROR");
	}

	return F("UNKNOWN");
}

/**
 * @brief Get a formatted time {@link String} based on the system time.
 * @return {@link String} representation of the system time
 */
String TesLight::Logger::getTimeString()
{
	unsigned long milli = millis();
	const unsigned long hour = milli / 3600000;
	milli = milli - 3600000 * hour;
	const unsigned long min = milli / 60000;
	milli = milli - 60000 * min;
	const unsigned long sec = milli / 1000;
	milli = milli - 1000 * sec;

	String timeString = F("");
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