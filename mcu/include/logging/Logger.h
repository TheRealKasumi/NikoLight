/**
 * @file Logger.h
 * @author TheRealKasumi
 * @brief Static class containing the {@link NL::Logger}.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <HardwareSerial.h>
#include <WString.h>
#include <FS.h>

#define SOURCE_LOCATION __FILE__, __func__, __LINE__

namespace NL
{

	class Logger
	{
	public:
		enum class LogLevel : uint8_t
		{
			DEBUG = 0,
			INFO = 1,
			WARN = 2,
			ERROR = 3
		};

		static bool begin(const NL::Logger::LogLevel minLogLevel = NL::Logger::LogLevel::INFO);
		static bool begin(const uint32_t baudRate, const NL::Logger::LogLevel minLogLevel = NL::Logger::LogLevel::INFO);
		static bool begin(FS *fs, const String fn, const NL::Logger::LogLevel minLogLevel = NL::Logger::LogLevel::INFO);
		static bool begin(uint32_t baudRate, FS *fs, const String fn, const NL::Logger::LogLevel minLogLevel = NL::Logger::LogLevel::INFO);
		static void end();
		static bool isInitialized();

		static void setMinLogLevel(const NL::Logger::LogLevel logLevel);

		static void log(const NL::Logger::LogLevel logLevel, const char *file, const char *function, const int line, const String message);

		static size_t getLogSize();
		static void readLog(uint8_t *buffer, const size_t start, const size_t bufferSize);
		static void clearLog();

	private:
		Logger();

		static bool initialized;
		static bool logToSerial;
		static bool logToFile;
		static FS *fileSystem;
		static String fileName;
		static NL::Logger::LogLevel minLogLevel;

		static bool testOpenFile(FS *fs, const String fn);
		static String getLogLevelString(const NL::Logger::LogLevel logLevel);
		static String getTimeString();
	};
}

#endif
