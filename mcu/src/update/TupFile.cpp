/**
 * @file TupFile.h
 * @author TheRealKasumi
 * @brief Contains the implementation for the {@link TL::TupFile}.
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
 * @return true when the file is opened
 * @return false when the file could not be opened
 */
bool TL::TupFile::load(FS *fileSystem, const String fileName)
{
	this->close();
	this->file = fileSystem->open(fileName, FILE_READ);
	if (!this->file)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open TUP file. The file was not found."));
		return false;
	}
	else if (this->file.isDirectory())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open TUP file. It is a directory."));
		this->close();
		return false;
	}

	if (!this->loadTupHeader())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TUP file header is invalid."));
		this->close();
		return false;
	}

	if (!this->verify())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to verify TUP file. The file is invalid or corrupted."));
		this->close();
		return false;
	}

	return true;
}

/**
 * @brief Unpack the content of the valid package to a specified folder on the file system.
 * @param fileSystem file system to which to data is unpacked
 * @param root root folder to which the data is unpacked
 * @return true when the unpacking was successful
 * @return false when there was an error unpacking the data
 */
bool TL::TupFile::unpack(FS *fileSystem, const String root)
{
	if (!this->file.seek(13))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to unpack TUP file. There is no content."));
		return false;
	}

	TL::TupFile::TupDataBlock dataBlock;
	dataBlock.type = TL::TupFile::TupDataType::FIRMWARE;
	dataBlock.path = new char[256];
	for (uint32_t i = 0; i < this->tupHeader.numberBlocks; i++)
	{
		this->file.read((uint8_t *)&dataBlock.type, 1);
		this->file.read((uint8_t *)&dataBlock.pathLength, 2);
		if (dataBlock.pathLength > 255)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Invalid name for data block ") + String(i) + F(". Can not continue."));
			delete[] dataBlock.path;
			return false;
		}
		this->file.read((uint8_t *)dataBlock.path, dataBlock.pathLength);
		this->file.read((uint8_t *)&dataBlock.size, 4);

		const String absolutePath = this->createAbsolutePath(root + F("/"), dataBlock.path, dataBlock.pathLength);
		if (dataBlock.type == TL::TupFile::TupDataType::DIRECTORY)
		{
			if (!fileSystem->mkdir(absolutePath))
			{
				TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to create directory \"") + absolutePath + F("\". Can not continue."));
				delete[] dataBlock.path;
				return false;
			}
		}
		else if (dataBlock.type == TL::TupFile::TupDataType::FILE || dataBlock.type == TL::TupFile::TupDataType::FIRMWARE)
		{
			File file = fileSystem->open(absolutePath, FILE_WRITE);
			if (!file)
			{
				TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to write file \"") + absolutePath + F("\". Can not continue."));
				delete[] dataBlock.path;
				return false;
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
				readBytes += chunkSize;

				uint32_t bytesWritten = 0;
				while (bytesWritten < chunkSize)
				{
					bytesWritten += file.write(buffer + bytesWritten, chunkSize - bytesWritten);
				}
			}

			file.close();
		}
	}

	delete[] dataBlock.path;
	return true;
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
 * @return true when the header was loaded successfully
 * @return false when there was an error loading the header
 */
bool TL::TupFile::loadTupHeader()
{
	if (!this->file)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Could not load TUP header because the file could not be opened."));
		return false;
	}
	else if (this->file.size() < 13)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Could not load TUP header. The file is invalid."));
		return false;
	}

	this->file.read((uint8_t *)this->tupHeader.magic, 4);
	this->file.read((uint8_t *)&this->tupHeader.fileVersion, 1);
	this->file.read((uint8_t *)&this->tupHeader.hash, 4);
	this->file.read((uint8_t *)&this->tupHeader.numberBlocks, 4);

	if (this->tupHeader.magic[0] != 'T' || this->tupHeader.magic[1] != 'L' || this->tupHeader.magic[2] != 'U' || this->tupHeader.magic[3] != 'P')
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load TUP header. Magic numbers are invalid."));
		return false;
	}

	if (this->tupHeader.fileVersion != 1)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load TUP header. Invalid file version."));
		return false;
	}

	if (this->tupHeader.numberBlocks == 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The number of data blocks is invalid. It must not be null."));
		return false;
	}

	return true;
}

/**
 * @brief Verify a {@link TL::TupFile}.
 * @return true when the file is valid
 * @return false when the file is invalid
 */
bool TL::TupFile::verify()
{
	return this->generateHash() == this->tupHeader.hash;
}

/**
 * @brief Generate the simple hash to verify the TUP file.
 * @return uint32_t hash
 */
uint32_t TL::TupFile::generateHash()
{
	if (!this->file.seek(13))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to generate hash for TUP file. There is no content."));
		return false;
	}

	TL::TupFile::TupDataBlock dataBlock;
	dataBlock.type = TL::TupFile::TupDataType::FIRMWARE;
	dataBlock.path = new char[256];
	uint32_t hash = 7;
	for (uint32_t i = 0; i < this->tupHeader.numberBlocks; i++)
	{
		this->file.read((uint8_t *)&dataBlock.type, 1);
		this->file.read((uint8_t *)&dataBlock.pathLength, 2);
		if (dataBlock.pathLength > 255)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The name of the data block is invalid."));
			delete[] dataBlock.path;
			return 0;
		}
		this->file.read((uint8_t *)dataBlock.path, dataBlock.pathLength <= 255 ? dataBlock.pathLength : 255);
		this->file.read((uint8_t *)&dataBlock.size, 4);

		hash = hash * 31 + dataBlock.type;
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
