/**
 * @file NupFile.h
 * @author TheRealKasumi
 * @brief Contains the implementation for the {@link NL::NupFile}.
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
#include "update/NupFile.h"

/**
 * @brief Create a new instance of {@link NL::NupFile}.
 */
NL::NupFile::NupFile()
{
	this->initHeader();
}

/**
 * @brief Destroy the {@link NL::NupFile} instance and free resources.
 */
NL::NupFile::~NupFile()
{
	this->close();
}

/**
 * @brief Open a NUP file and verify it.
 * @param fileSystem file system where the file is located
 * @param fileName name of the file to open
 * @return OK when the update file was loaded
 * @return ERROR_FILE_NOT_FOUND when the file was not found
 * @return ERROR_FILE_READ when the file could not be read
 * @return ERROR_IS_DIRECTORY when a directory instead of a file was found
 * @return ERROR_INVALID_HEADER when the NUP header is invalid
 * @return ERROR_INVALID_DATA when the file data is invalid or corrupted
 * @return ERROR_EMPTY_FILE when the file continas no data
 * @return ERROR_MAGIC_NUMBERS when the magic numbers are invalid
 * @return ERROR_FILE_VERSION when the file version does not match
 * @return ERROR_FILE_HASH the file hash is invalid
 */
NL::NupFile::Error NL::NupFile::load(FS *fileSystem, const String fileName)
{
	this->close();
	this->file = fileSystem->open(fileName, FILE_READ);
	if (!this->file)
	{
		return NL::NupFile::Error::ERROR_FILE_NOT_FOUND;
	}
	else if (this->file.isDirectory())
	{
		this->close();
		return NL::NupFile::Error::ERROR_IS_DIRECTORY;
	}

	const NL::NupFile::Error headerError = this->loadNupHeader();
	if (headerError != NL::NupFile::Error::OK)
	{
		this->close();
		return headerError;
	}

	const NL::NupFile::Error verifyError = this->verify();
	if (verifyError != NL::NupFile::Error::OK)
	{
		this->close();
		return verifyError;
	}

	return NL::NupFile::Error::OK;
}

/**
 * @brief Unpack the content of the valid package to a specified folder on the file system.
 * @param fileSystem file system to which to data is unpacked
 * @param root root folder to which the data is unpacked
 * @return OK when the unpacking was successful
 * @return ERROR_EMPTY_FILE when the NUP has no data
 * @return ERROR_FILE_READ when the file could not be read
 * @return ERROR_INVALID_BLOCK_NAME the NUP contains a invalid block name
 * @return ERROR_CREATE_DIR when a directory could not be created while unpacking
 * @return ERROR_CREATE_FILE when a file could not be created while unpacking
 */
NL::NupFile::Error NL::NupFile::unpack(FS *fileSystem, const String root)
{
	if (!this->file.seek(13))
	{
		return NL::NupFile::Error::ERROR_EMPTY_FILE;
	}

	NL::NupFile::NupDataBlock dataBlock;
	dataBlock.type = NL::NupFile::NupDataType::FIRMWARE;
	dataBlock.path = new char[256];
	for (uint32_t i = 0; i < this->nupHeader.numberBlocks; i++)
	{
		bool readError = false;
		readError = this->file.read((uint8_t *)&dataBlock.type, 1) != 1 ? true : readError;
		readError = this->file.read((uint8_t *)&dataBlock.pathLength, 2) != 2 ? true : readError;
		if (dataBlock.pathLength > 255)
		{
			delete[] dataBlock.path;
			return NL::NupFile::Error::ERROR_INVALID_BLOCK_NAME;
		}
		readError = this->file.read((uint8_t *)dataBlock.path, dataBlock.pathLength) != dataBlock.pathLength ? true : readError;
		readError = this->file.read((uint8_t *)&dataBlock.size, 4) != 4 ? true : readError;

		const String absolutePath = this->createAbsolutePath(root + F("/"), dataBlock.path, dataBlock.pathLength);
		if (dataBlock.type == NL::NupFile::NupDataType::DIRECTORY)
		{
			if (!fileSystem->mkdir(absolutePath))
			{
				delete[] dataBlock.path;
				return NL::NupFile::Error::ERROR_CREATE_DIR;
			}
		}
		else if (dataBlock.type == NL::NupFile::NupDataType::FILE || dataBlock.type == NL::NupFile::NupDataType::FIRMWARE)
		{
			File file = fileSystem->open(absolutePath, FILE_WRITE);
			if (!file)
			{
				delete[] dataBlock.path;
				return NL::NupFile::Error::ERROR_CREATE_FILE;
			}

			uint8_t buffer[1024];
			uint32_t readBytes = 0;
			while (readBytes < dataBlock.size)
			{
				uint32_t chunkSize = dataBlock.size - readBytes;
				if (chunkSize > 1024)
				{
					chunkSize = 1024;
				}

				chunkSize = this->file.read(buffer, chunkSize);
				if (chunkSize == 0)
				{
					return NL::NupFile::Error::ERROR_FILE_READ;
				}
				readBytes += chunkSize;

				uint32_t bytesWritten = 0;
				while (bytesWritten < chunkSize)
				{
					bytesWritten += file.write(buffer + bytesWritten, chunkSize - bytesWritten);
				}
			}

			file.close();
		}

		if (readError)
		{
			return NL::NupFile::Error::ERROR_FILE_READ;
		}
	}

	delete[] dataBlock.path;
	return NL::NupFile::Error::OK;
}

/**
 * @brief Close the NUP file when one is opened.
 */
void NL::NupFile::close()
{
	this->initHeader();
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Get the currently loaded {@link NL::NupFile::NupHeader}.
 * @return header of the loaded file
 */
NL::NupFile::NupHeader NL::NupFile::getHeader()
{
	return this->nupHeader;
}

/**
 * @brief Initialize the NUP header.
 */
void NL::NupFile::initHeader()
{
	this->nupHeader.magic[0] = 0;
	this->nupHeader.magic[1] = 0;
	this->nupHeader.magic[2] = 0;
	this->nupHeader.magic[3] = 0;
	this->nupHeader.fileVersion = 0;
	this->nupHeader.hash = 0;
	this->nupHeader.numberBlocks = 0;
}

/**
 * @brief Load the NUP header from a opened NUP file.
 * @return OK when the header was loaded successfully
 * @return ERROR_FILE_NOT_FOUND when there was an error opening the file
 * @return ERROR_EMPTY_FILE when the file continas no data
 * @return ERROR_MAGIC_NUMBERS when the magic numbers are invalid
 * @return ERROR_FILE_VERSION when the file version does not match
 * @return ERROR_FILE_READ when the file could not be read
 */
NL::NupFile::Error NL::NupFile::loadNupHeader()
{
	if (!this->file)
	{
		return NL::NupFile::Error::ERROR_FILE_NOT_FOUND;
	}
	else if (this->file.size() < 13)
	{
		return NL::NupFile::Error::ERROR_EMPTY_FILE;
	}

	bool readError = false;
	readError = this->file.read((uint8_t *)this->nupHeader.magic, 4) == 4 ? readError : true;
	readError = this->file.read((uint8_t *)&this->nupHeader.fileVersion, 1) == 1 ? readError : true;
	readError = this->file.read((uint8_t *)&this->nupHeader.hash, 4) == 4 ? readError : true;
	readError = this->file.read((uint8_t *)&this->nupHeader.numberBlocks, 4) == 4 ? readError : true;
	if (readError)
	{
		return NL::NupFile::Error::ERROR_FILE_READ;
	}

	if (this->nupHeader.magic[0] != 'N' || this->nupHeader.magic[1] != 'L' || this->nupHeader.magic[2] != 'U' || this->nupHeader.magic[3] != 'P')
	{
		return NL::NupFile::Error::ERROR_MAGIC_NUMBERS;
	}

	if (this->nupHeader.fileVersion != 1)
	{
		return NL::NupFile::Error::ERROR_FILE_VERSION;
	}

	if (this->nupHeader.numberBlocks == 0)
	{
		return NL::NupFile::Error::ERROR_EMPTY_FILE;
	}

	return NL::NupFile::Error::OK;
}

/**
 * @brief Verify a {@link NL::NupFile}.
 * @return OK when the file is valid
 * @return ERROR_FILE_HASH when the file is invalid
 */
NL::NupFile::Error NL::NupFile::verify()
{
	return this->generateHash() == this->nupHeader.hash ? NL::NupFile::Error::OK : NL::NupFile::Error::ERROR_FILE_HASH;
}

/**
 * @brief Generate the simple hash to verify the NUP file.
 * @return hash
 */
uint32_t NL::NupFile::generateHash()
{
	if (!this->file.seek(13))
	{
		return 0;
	}

	NL::NupFile::NupDataBlock dataBlock;
	dataBlock.type = NL::NupFile::NupDataType::NONE;
	dataBlock.path = new char[256];
	uint32_t hash = 7;
	for (uint32_t i = 0; i < this->nupHeader.numberBlocks; i++)
	{
		bool readError = false;
		readError = this->file.read((uint8_t *)&dataBlock.type, 1) == 1 ? readError : true;
		readError = this->file.read((uint8_t *)&dataBlock.pathLength, 2) == 2 ? readError : true;
		if (dataBlock.pathLength > 255)
		{
			delete[] dataBlock.path;
			return 0;
		}
		readError = this->file.read((uint8_t *)dataBlock.path, dataBlock.pathLength <= 255 ? dataBlock.pathLength : 255) == (dataBlock.pathLength <= 255 ? dataBlock.pathLength : 255) ? readError : true;
		readError = this->file.read((uint8_t *)&dataBlock.size, 4) == 4 ? readError : true;

		if (readError)
		{
			return 0;
		}

		hash = hash * 31 + static_cast<uint8_t>(dataBlock.type);
		hash = hash * 31 + dataBlock.pathLength;

		for (uint32_t j = 0; j < dataBlock.pathLength; j++)
		{
			hash = hash * 31 + dataBlock.path[j];
		}

		hash = hash * 31 + dataBlock.size;

		uint8_t buffer[1024];
		uint32_t readBytes = 0;
		while (readBytes < dataBlock.size)
		{
			uint32_t chunkSize = dataBlock.size - readBytes;
			if (chunkSize > 1024)
			{
				chunkSize = 1024;
			}

			chunkSize = this->file.read(buffer, chunkSize);
			if (chunkSize == 0)
			{
				return 0;
			}

			readBytes += chunkSize;
			for (uint16_t j = 0; j < chunkSize; j++)
			{
				hash = hash * 31 + buffer[j];
			}
		}
	}

	delete[] dataBlock.path;
	return hash;
}

/**
 * @brief Construct the absolute path based on the extraction root and the name of the data block.
 * @param root root path
 * @param path relative path to root from TUP
 * @param pathLength length of the realative path
 * @return String absolute path
 */
String NL::NupFile::createAbsolutePath(const String root, const char *name, uint16_t nameLength)
{
	String absolutePath = root;
	for (uint16_t i = 0; i < nameLength; i++)
	{
		if (name[i] != '\\')
		{
			absolutePath += name[i];
		}
		else
		{
			absolutePath += '/';
		}
	}
	return absolutePath;
}
