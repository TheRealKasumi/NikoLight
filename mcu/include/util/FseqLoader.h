/**
 * @file FseqLoader.h
 * @author TheRealKasumi
 * @brief Contains a class to load and verify fseq 1.0 files created by xLights.
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
#ifndef FSEQ_LOADER_H
#define FSEQ_LOADER_H

#include <stdint.h>
#include <vector>
#include <FS.h>

#include "led/driver/LedStrip.h"

namespace TL
{
	class FseqLoader
	{
	public:
		enum class Error
		{
			OK,						   // No error
			ERROR_FILE_NOT_FOUND,	   // The file was not found
			ERROR_FILE_IS_DIR,		   // The file is not a file but a directory
			ERROR_FILE_TOO_SMALL,	   // The file is empty or too small to be valid
			ERROR_FILE_READ,		   // The file could not be read
			ERROR_MAGIC_NUMBERS,	   // The magic numbers do not fit the file
			ERROR_FILE_VERSION,		   // The file version is not supported
			ERROR_HEADER_LENGTH,	   // The header length of the file is invalid
			ERROR_INVALID_DATA_LENGTH, // The data length specified in header does not match actual data length
			ERROR_END_OF_FILE		   // End of the file was reached
		};

		struct FseqHeader
		{
			char identifier[4];
			uint16_t channelDataOffset;
			uint8_t minorVersion;
			uint8_t majorVersion;
			uint16_t headerLength;
			uint32_t channelCount;
			uint32_t frameCount;
			uint8_t stepTime;
			uint8_t flags;
			uint16_t universeCount;
			uint16_t universeSize;
			uint8_t gamma;
			uint8_t colorEncoding;
			uint16_t reserved;
		};

		FseqLoader(FS *fileSystem);
		~FseqLoader();

		TL::FseqLoader::Error loadFromFile(const String fileName);
		size_t available();
		void moveToStart();
		void close();

		FseqHeader getHeader();
		TL::FseqLoader::Error readLedStrip(TL::LedStrip &ledStrip);

		void setFillerBytes(const uint8_t fillerBytes);
		uint8_t getFillerBytes();

		void setZoneCount(const uint8_t zoneCount);
		uint8_t getZoneCount();

	private:
		FS *fileSystem;
		File file;
		FseqHeader fseqHeader;
		uint8_t fillerBytes;
		uint8_t zoneCount;
		uint8_t zoneCounter;

		void initFseqHeader();
		TL::FseqLoader::Error isValid();
	};
}

#endif