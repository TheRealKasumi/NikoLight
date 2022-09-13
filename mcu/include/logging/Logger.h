/**
 * @file Logger.h
 * @author TheRealKasumi
 * @brief Static class containing the {@link TesLight::Logger}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <FS.h>

namespace TesLight
{

	class Logger
	{
	public:
		enum LogLevel
		{
			DEBUG = 0,
			INFO = 1,
			WARN = 2,
			ERROR = 3
		};

		static bool begin();
		static bool begin(const uint32_t baudRate);
		static bool begin(FS *fs, const String fn);
		static bool begin(uint32_t baudRate, FS *fs, const String fn);

		static void setMinLogLevel(const TesLight::Logger::LogLevel logLevel);

		static void log(const TesLight::Logger::LogLevel logLevel, const String source, const String message);
		static void logLn(const TesLight::Logger::LogLevel logLevel, const String source, const String message);

		static size_t getLogSize();
		static void readLog(uint8_t *buffer, const size_t start, const size_t bufferSize);
		static void clearLog();

	private:
		static bool logToSerial;
		static bool logToFile;
		static FS *fileSystem;
		static String fileName;
		static TesLight::Logger::LogLevel minLogLevel;

		Logger(){};

		static bool testOpenFile(FS *fs, const String fn);
		static String getLogLevelString(const TesLight::Logger::LogLevel logLevel);
		static String getTimeString();
	};
}

#endif
