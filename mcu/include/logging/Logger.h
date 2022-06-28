/**
 * @file Logger.h
 * @author TheRealKasumi
 * @brief Contains a class to log information to the serial monitor or sd card.
 * @version 0.0.1
 * @date 2022-06-28
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
		Logger();
		Logger(const uint32_t baudRate);
		Logger(const uint32_t baudRate, FS *fileSystem, const String fileName);
		~Logger();

		bool begin();

		void info(const String source, const String message);
		void warn(const String source, const String message);
		void error(const String source, const String message);

		size_t getLogSize();
		void readLog(uint8_t *buffer, const size_t start, const size_t bufferSize);
		void clearLog();

	private:
		uint32_t baudRate;
		FS *fileSystem;
		String fileName;

		void log(const String logLevel, const String source, const String message);
		String getTimeString();
	};
}

#endif
