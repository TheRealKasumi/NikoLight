/**
 * @file TupFile.h
 * @author TheRealKasumi
 * @brief Contains the implementation for the {@link TL::TupFile}.
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
#include "update/TupFile.h"

/**
 * @brief Create a new instance of {@link TL::TupFile}.
 */
TL::TupFile::TupFile()
{
	this->initHeader();
}

/**
 * @brief Destroy the {@link TL::TupFile} instance and free resources.
 */
TL::TupFile::~TupFile()
{
	this->close();
}

/**
 * @brief Open a TUP file and verify it.
 * @param fileSystem file system where the file is located
 * @param fileName name of the file to open
 * @return OK when the update file was loaded
 * @return ERROR_FILE_NOT_FOUND when the file was not found
 * @return ERROR_FILE_READ when the file could not be read
 * @return ERROR_IS_DIRECTORY when a directory instead of a file was found
 * @return ERROR_INVALID_HEADER when the TUP header is invalid
 * @return ERROR_INVALID_DATA when the file data is invalid or corrupted
 * @return ERROR_EMPTY_FILE when the file continas no data
 * @return ERROR_MAGIC_NUMBERS when the magic numbers are invalid
 * @return ERROR_FILE_VERSION when the file version does not match
 * @return ERROR_FILE_HASH the file hash is invalid
 */
TL::TupFile::Error TL::TupFile::load(FS *fileSystem, const String fileName)
{
	this->close();
	this->file = fileSystem->open(fileName, FILE_READ);
	if (!this->file)
	{
		return TL::TupFile::Error::ERROR_FILE_NOT_FOUND;
	}
	else if (this->file.isDirectory())
	{
		this->close();
		return TL::TupFile::Error::ERROR_IS_DIRECTORY;
	}

	const TL::TupFile::Error headerError = this->loadTupHeader();
	if (headerError != TL::TupFile::Error::OK)
	{
		this->close();
		return headerError;
	}

	const TL::TupFile::Error verifyError = this->verify();
	if (verifyError != TL::TupFile::Error::OK)
	{
		this->close();
		return verifyError;
	}

	return TL::TupFile::Error::OK;
}

/**
 * @brief Unpack the content of the valid package to a specified folder on the file system.
 * @param fileSystem file system to which to data is unpacked
 * @param root root folder to which the data is unpacked
 * @return OK when the unpacking was successful
 * @return ERROR_EMPTY_FILE when the TUP has no data
 * @return ERROR_FILE_READ when the file could not be read
 * @return ERROR_INVALID_BLOCK_NAME the TUP contains a invalid block name
 * @return ERROR_CREATE_DIR when a directory could not be created while unpacking
 * @return ERROR_CREATE_FILE when a file could not be created while unpacking
 */
TL::TupFile::Error TL::TupFile::unpack(FS *fileSystem, const String root)
{
	if (!this->file.seek(13))
	{
		return TL::TupFile::Error::ERROR_EMPTY_FILE;
	}

	TL::TupFile::TupDataBlock dataBlock;
	dataBlock.type = TL::TupFile::TupDataType::FIRMWARE;
	dataBlock.path = new char[256];
	for (uint32_t i = 0; i < this->tupHeader.numberBlocks; i++)
	{
		bool readError = false;
		readError = this->file.read((uint8_t *)&dataBlock.type, 1) != 1 ? true : readError;
		readError = this->file.read((uint8_t *)&dataBlock.pathLength, 2) != 2 ? true : readError;
		if (dataBlock.pathLength > 255)
		{
			delete[] dataBlock.path;
			return TL::TupFile::Error::ERROR_INVALID_BLOCK_NAME;
		}
		readError = this->file.read((uint8_t *)dataBlock.path, dataBlock.pathLength) != dataBlock.pathLength ? true : readError;
		readError = this->file.read((uint8_t *)&dataBlock.size, 4) != 4 ? true : readError;

		const String absolutePath = this->createAbsolutePath(root + F("/"), dataBlock.path, dataBlock.pathLength);
		if (dataBlock.type == TL::TupFile::TupDataType::DIRECTORY)
		{
			if (!fileSystem->mkdir(absolutePath))
			{
				delete[] dataBlock.path;
				return TL::TupFile::Error::ERROR_CREATE_DIR;
			}
		}
		else if (dataBlock.type == TL::TupFile::TupDataType::FILE || dataBlock.type == TL::TupFile::TupDataType::FIRMWARE)
		{
			File file = fileSystem->open(absolutePath, FILE_WRITE);
			if (!file)
			{
				delete[] dataBlock.path;
				return TL::TupFile::Error::ERROR_CREATE_FILE;
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
					return TL::TupFile::Error::ERROR_FILE_READ;
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
			return TL::TupFile::Error::ERROR_FILE_READ;
		}
	}

	delete[] dataBlock.path;
	return TL::TupFile::Error::OK;
}

/**
 * @brief Close the TUP file when one is opened.
 */
void TL::TupFile::close()
{
	this->initHeader();
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Get the currently loaded {@link TL::TupFile::TupHeader}.
 * @return header of the loaded file
 */
TL::TupFile::TupHeader TL::TupFile::getHeader()
{
	return this->tupHeader;
}

/**
 * @brief Initialize the TUP header.
 */
void TL::TupFile::initHeader()
{
	this->tupHeader.magic[0] = 0;
	this->tupHeader.magic[1] = 0;
	this->tupHeader.magic[2] = 0;
	this->tupHeader.magic[3] = 0;
	this->tupHeader.fileVersion = 0;
	this->tupHeader.hash = 0;
	this->tupHeader.numberBlocks = 0;
}

/**
 * @brief Load the TUP header from a opened TUP file.
 * @return OK when the header was loaded successfully
 * @return ERROR_FILE_NOT_FOUND when there was an error opening the file
 * @return ERROR_EMPTY_FILE when the file continas no data
 * @return ERROR_MAGIC_NUMBERS when the magic numbers are invalid
 * @return ERROR_FILE_VERSION when the file version does not match
 * @return ERROR_FILE_READ when the file could not be read
 */
TL::TupFile::Error TL::TupFile::loadTupHeader()
{
	if (!this->file)
	{
		return TL::TupFile::Error::ERROR_FILE_NOT_FOUND;
	}
	else if (this->file.size() < 13)
	{
		return TL::TupFile::Error::ERROR_EMPTY_FILE;
	}

	bool readError = false;
	readError = this->file.read((uint8_t *)this->tupHeader.magic, 4) == 4 ? readError : true;
	readError = this->file.read((uint8_t *)&this->tupHeader.fileVersion, 1) == 1 ? readError : true;
	readError = this->file.read((uint8_t *)&this->tupHeader.hash, 4) == 4 ? readError : true;
	readError = this->file.read((uint8_t *)&this->tupHeader.numberBlocks, 4) == 4 ? readError : true;
	if (readError)
	{
		return TL::TupFile::Error::ERROR_FILE_READ;
	}

	if (this->tupHeader.magic[0] != 'T' || this->tupHeader.magic[1] != 'L' || this->tupHeader.magic[2] != 'U' || this->tupHeader.magic[3] != 'P')
	{
		return TL::TupFile::Error::ERROR_MAGIC_NUMBERS;
	}

	if (this->tupHeader.fileVersion != 1)
	{
		return TL::TupFile::Error::ERROR_FILE_VERSION;
	}

	if (this->tupHeader.numberBlocks == 0)
	{
		return TL::TupFile::Error::ERROR_EMPTY_FILE;
	}

	return TL::TupFile::Error::OK;
}

/**
 * @brief Verify a {@link TL::TupFile}.
 * @return OK when the file is valid
 * @return ERROR_FILE_HASH when the file is invalid
 */
TL::TupFile::Error TL::TupFile::verify()
{
	return this->generateHash() == this->tupHeader.hash ? TL::TupFile::Error::OK : TL::TupFile::Error::ERROR_FILE_HASH;
}

/**
 * @brief Generate the simple hash to verify the TUP file.
 * @return hash
 */
uint32_t TL::TupFile::generateHash()
{
	if (!this->file.seek(13))
	{
		return 0;
	}

	TL::TupFile::TupDataBlock dataBlock;
	dataBlock.type = TL::TupFile::TupDataType::NONE;
	dataBlock.path = new char[256];
	uint32_t hash = 7;
	for (uint32_t i = 0; i < this->tupHeader.numberBlocks; i++)
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
String TL::TupFile::createAbsolutePath(const String root, const char *name, uint16_t nameLength)
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
