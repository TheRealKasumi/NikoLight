/**
 * @file TupFile.h
 * @author TheRealKasumi
 * @brief Contains a class for loading a TesLight Update Package file.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef TUP_FILE_H
#define TUP_FILE_H

#include <Arduino.h>
#include <FS.h>
#include "logging/Logger.h"

namespace TL
{
	class TupFile
	{
	public:
		struct TupHeader
		{
			char magic[4];
			uint8_t fileVersion;
			uint32_t hash;
			uint32_t numberBlocks;
		};

		enum TupDataType
		{
			FIRMWARE = 0,
			FILE = 1,
			DIRECTORY = 2
		};

		struct TupDataBlock
		{
			TupDataType type;
			uint16_t pathLength;
			char *path;
			uint32_t size;
			uint8_t *data;
		};

		TupFile();
		~TupFile();

		bool load(FS *fileSystem, const String fileName);
		bool unpack(FS *fileSystem, const String root);
		void close();

		TL::TupFile::TupHeader getHeader();

	private:
		File file;
		TL::TupFile::TupHeader tupHeader;

		void initHeader();
		bool loadTupHeader();

		bool verify();
		uint32_t generateHash();

		String createAbsolutePath(const String root, const char *name, uint16_t nameLength);
	};
}

#endif