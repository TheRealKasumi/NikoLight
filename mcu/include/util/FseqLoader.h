/**
 * @file FseqLoader.h
 * @author TheRealKasumi
 * @brief Contains a class to load and verify fseq 1.0 files created by xLights.
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
#ifndef FSEQ_LOADER_H
#define FSEQ_LOADER_H

#include <stdint.h>
#include <vector>
#include <FS.h>

#include "led/driver/LedStrip.h"

namespace NL
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

		NL::FseqLoader::Error loadFromFile(const String fileName);
		size_t available();
		void moveToStart();
		void close();

		FseqHeader getHeader();
		NL::FseqLoader::Error readLedStrip(NL::LedStrip &ledStrip);

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
		NL::FseqLoader::Error isValid();
	};
}

#endif