/**
 * @file InMemoryBinaryFile.h
 * @author TheRealKasumi
 * @brief Class to read and write data in binary format in memory.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef IN_MEMORY_BINARY_FILE_H
#define IN_MEMORY_BINARY_FILE_H

#include <Arduino.h>

#include "logging/Logger.h"

namespace TesLight
{
	class InMemoryBinaryFile
	{
	public:
		InMemoryBinaryFile(const size_t size, const bool freeOnDestory = true);
		~InMemoryBinaryFile();

		uint8_t *getData();
		bool loadFrom(uint8_t *buffer, const size_t bufferSize);

		size_t getBytesWritten();

		/**
		 * @brief Write a value to the binary file.
		 * @param value value to write
		 * @return true when succesful
		 * @return false when there was an error
		 */
		template <typename T>
		bool write(const T value)
		{
			if (this->index + sizeof(value) - 1 >= this->size)
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to write to in memory binary File because the data exceeds the buffer size."));
				return false;
			}

			for (uint16_t i = 0; i < sizeof(value); i++)
			{
				this->buffer[this->index++] = ((uint8_t *)&value)[i];
			}

			return true;
		}

		/**
		 * @brief Read a value from the file.
		 * @param value reference to the variable which will hold the value
		 * @return true when successful
		 * @return false when there was an error
		 */
		template <typename T>
		bool read(T &value)
		{
			if (this->index + sizeof(value) - 1 >= this->size)
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read from in memory binary File because the end is reached."));
				return false;
			}

			uint8_t *valuePtr = (uint8_t *)&value;
			for (uint16_t i = 0; i < sizeof(value); i++)
			{
				valuePtr[i] = this->buffer[this->index++];
			}

			return true;
		}

		bool writeString(const String string);
		bool readString(String &string);

	private:
		uint8_t *buffer;
		size_t size;
		size_t index;
		bool freeOnDestroy;
	};
}

#endif