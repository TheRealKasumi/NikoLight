/**
 * @file Logger.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::Logger}.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "logging/Logger.h"

/**
 * @brief Create a new instance of {@link TesLight::Logger}.
 */
TesLight::Logger::Logger()
{
	this->baudRate = 115200;
	this->fileSystem = nullptr;
	this->fileName = F("");
}

/**
 * @brief Create a new instance of {@link TesLight::Logger} with a given baud rate.
 * @param baudRate baud rate for the serial connection
 */
TesLight::Logger::Logger(const uint32_t baudRate)
{
	this->baudRate = baudRate;
	this->fileSystem = nullptr;
	this->fileName = F("");
}

/**
 * @brief Create a new instance of {@link TesLight::Logger}.
 * @param baudRate baudrate for the serial connection
 * @param fileSystem instsance of {@link FS} for the logfile
 * @param fileName full file name of the log file
 */
TesLight::Logger::Logger(const uint32_t baudRate, FS *fileSystem, const String fileName)
{
	this->baudRate = baudRate;
	this->fileSystem = fileSystem;
	this->fileName = fileName;
}

/**
 * @brief Destroy the {@link TesLight::Logger} instance.
 */
TesLight::Logger::~Logger()
{
}

/**
 * @brief Initialize the {@link TesLight::Logger}.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::Logger::begin()
{
	Serial.begin(this->baudRate);

	if (this->fileSystem != nullptr)
	{
		File logFile = this->fileSystem->open(this->fileName, FILE_APPEND);
		if (!logFile)
		{
			error(F("Logger.cpp:begin"), F("Failed to open log file."));
			return false;
		}
		logFile.close();
	}

	info(F("Logger.cpp:begin"), F("Logger initialized successfully."));
	return true;
}

/**
 * @brief Print information messages.
 * @param source source of the message
 * @param message message
 */
void TesLight::Logger::info(const String source, const String message)
{
	log(F("info"), source, message);
}

/**
 * @brief Print warning messages.
 * @param source source of the message
 * @param message message
 */
void TesLight::Logger::warn(const String source, const String message)
{
	log(F("warning"), source, message);
}

/**
 * @brief Print error messages.
 * @param source source of the message
 * @param message message
 */
void TesLight::Logger::error(const String source, const String message)
{
	log(F("error"), source, message);
}

/**
 * @brief Get the size of the log on the sd card.
 * @return size_t number of bytes
 */
size_t TesLight::Logger::getLogSize()
{
	if (this->fileSystem == nullptr)
	{
		return 0;
	}

	File logFile = this->fileSystem->open(fileName, FILE_READ);
	if (!logFile)
	{
		return 0;
	}

	const size_t logSize = logFile.size();
	logFile.close();
	return logSize;
}

/**
 * @brief Read a number of bytes from the log on the sd card.
 * @param buffer buffer to hold the log data
 * @param start start position to read from
 * @param bufferSize size of the buffer / number of bytes to read
 */
void TesLight::Logger::readLog(uint8_t *buffer, const size_t start, const size_t bufferSize)
{
	if (this->fileSystem == nullptr)
	{
		return;
	}

	File logFile = this->fileSystem->open(fileName, FILE_READ);
	if (!logFile)
	{
		return;
	}

	logFile.seek(start);
	logFile.read(buffer, bufferSize);
	logFile.close();
}

/**
 * @brief Clear the log.
 */
void TesLight::Logger::clearLog()
{
	if (this->fileSystem == nullptr)
	{
		return;
	}

	this->fileSystem->remove(fileName);
}

/**
 * @brief Log a message to the serial monitor and sd card when connected.
 * @param logLevel level of the log message
 * @param source source of the message
 * @param message message
 */
void TesLight::Logger::log(const String logLevel, const String source, const String message)
{
	const String logString = getTimeString() + F(" [") + logLevel + F("] (") + source + F("): ") + message + F("\r\n");
	Serial.print(logString);

	if (this->fileSystem == nullptr)
	{
		return;
	}

	File logFile = this->fileSystem->open(fileName, FILE_APPEND);
	if (!logFile)
	{
		return;
	}
	logFile.write((uint8_t *)logString.c_str(), logString.length());
	logFile.close();
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
