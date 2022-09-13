/**
 * @file InMemoryBinaryFile.h
 * @author TheRealKasumi
 * @brief Class to read and write data in binary format in memory.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef IN_MEMORY_BINARY_FILE
#define IN_MEMORY_BINARY_FILE

#include <Arduino.h>

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

		bool writeByte(const uint8_t byte);
		uint8_t readByte();

		bool writeWord(const uint16_t word);
		uint16_t readWord();

		bool writeString(const String string);
		String readString();

	private:
		uint8_t *buffer;
		size_t size;
		size_t index;
		bool freeOnDestroy;
	};
}

#endif