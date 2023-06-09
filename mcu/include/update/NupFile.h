/**
 * @file NupFile.h
 * @author TheRealKasumi
 * @brief Contains a class for loading a NikoLight Update Package file.
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
#ifndef NUP_FILE_H
#define NUP_FILE_H

#include <stdint.h>
#include <WString.h>
#include <FS.h>

namespace NL
{
	class NupFile
	{
	public:
		enum class Error
		{
			OK,						  // No error
			ERROR_FILE_NOT_FOUND,	  // The file was not found
			ERROR_IS_DIRECTORY,		  // A directory instead of a file was found
			ERROR_INVALID_HEADER,	  // The NUP header is invalid
			ERROR_INVALID_DATA,		  // The NUP data is invalid
			ERROR_EMPTY_FILE,		  // The NUP has no content
			ERROR_FILE_READ,		  // The file could not be read
			ERROR_INVALID_BLOCK_NAME, // The NUP has a invalid data block name
			ERROR_CREATE_DIR,		  // One of the directories could not be created while unpacking
			ERROR_CREATE_FILE,		  // One of the files could not be created while unpacking
			ERROR_MAGIC_NUMBERS,	  // One of the magic numbers in the file header is invalid
			ERROR_FILE_VERSION,		  // The file version is invalid
			ERROR_FILE_HASH			  // The file hash is invalid
		};

		struct NupHeader
		{
			char magic[4];
			uint8_t fileVersion;
			uint32_t hash;
			uint32_t numberBlocks;
		};

		enum class NupDataType : uint8_t
		{
			FIRMWARE = 0,
			FILE = 1,
			DIRECTORY = 2,
			NONE = 255
		};

		struct NupDataBlock
		{
			NupDataType type;
			uint16_t pathLength;
			char *path;
			uint32_t size;
			uint8_t *data;
		};

		NupFile();
		~NupFile();

		NL::NupFile::Error load(FS *fileSystem, const String fileName);
		NL::NupFile::Error unpack(FS *fileSystem, const String root);
		void close();

		NL::NupFile::NupHeader getHeader();

	private:
		File file;
		NL::NupFile::NupHeader nupHeader;

		void initHeader();
		NL::NupFile::Error loadNupHeader();

		NL::NupFile::Error verify();
		uint32_t generateHash();

		String createAbsolutePath(const String root, const char *name, uint16_t nameLength);
	};
}

#endif