/**
 * @file FseqLoader.h
 * @author TheRealKasumi
 * @brief Contains a class to load and verify fseq 1.0 files created by xLights.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef FSEQ_LOADER_H
#define FSEQ_LOADER_H

#include <stdint.h>
#include <FS.h>

#include "logging/Logger.h"

#include "FastLED.h"

namespace TesLight
{
	class FseqLoader
	{
	public:
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

		bool loadFromFile(const String fileName);

		size_t available();
		void moveToStart();
		void close();

		FseqHeader getHeader();
		bool readPixelbuffer(CRGB *pixelBuffer, const size_t bufferSize);

	private:
		FS *fileSystem;
		File file;
		FseqHeader fseqHeader;

		void initFseqHeader();
		bool isValid();
	};
}

#endif