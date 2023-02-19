/**
 * @file TupFile.h
 * @author TheRealKasumi
 * @brief Contains a class for loading a TesLight Update Package file.
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
#ifndef TUP_FILE_H
#define TUP_FILE_H

#include <stdint.h>
#include <WString.h>
#include <FS.h>

namespace TL
{
	class TupFile
	{
	public:
		enum class Error
		{
			OK,						  // No error
			ERROR_FILE_NOT_FOUND,	  // The file was not found
			ERROR_IS_DIRECTORY,		  // A directory instead of a file was found
			ERROR_INVALID_HEADER,	  // The TUP header is invalid
			ERROR_INVALID_DATA,		  // The TUP data is invalid
			ERROR_EMPTY_FILE,		  // The TUP has no content
			ERROR_FILE_READ,		  // The file could not be read
			ERROR_INVALID_BLOCK_NAME, // The TUP has a invalid data block name
			ERROR_CREATE_DIR,		  // One of the directories could not be created while unpacking
			ERROR_CREATE_FILE,		  // One of the files could not be created while unpacking
			ERROR_MAGIC_NUMBERS,	  // One of the magic numbers in the file header is invalid
			ERROR_FILE_VERSION,		  // The file version is invalid
			ERROR_FILE_HASH			  // The file hash is invalid
		};

		struct TupHeader
		{
			char magic[4];
			uint8_t fileVersion;
			uint32_t hash;
			uint32_t numberBlocks;
		};

		enum class TupDataType : uint8_t
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

		TL::TupFile::Error load(FS *fileSystem, const String fileName);
		TL::TupFile::Error unpack(FS *fileSystem, const String root);
		void close();

		TL::TupFile::TupHeader getHeader();

	private:
		File file;
		TL::TupFile::TupHeader tupHeader;

		void initHeader();
		TL::TupFile::Error loadTupHeader();

		TL::TupFile::Error verify();
		uint32_t generateHash();

		String createAbsolutePath(const String root, const char *name, uint16_t nameLength);
	};
}

#endif