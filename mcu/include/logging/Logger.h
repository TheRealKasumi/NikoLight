/**
 * @file Logger.h
 * @author TheRealKasumi
 * @brief Static class containing the {@link NL::Logger}.
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
