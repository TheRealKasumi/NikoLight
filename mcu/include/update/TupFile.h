/**
 * @file TupFile.h
 * @author TheRealKasumi
 * @brief Contains a class for loading a TesLight Update Package file.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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