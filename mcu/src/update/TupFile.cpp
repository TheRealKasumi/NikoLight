/**
 * @file TupFile.h
 * @author TheRealKasumi
 * @brief Contains the implementation for the {@link TesLight::TupFile}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "update/TupFile.h"

/**
 * @brief Create a new instance of {@link TesLight::TupFile}.
 */
TesLight::TupFile::TupFile()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize TUP File."));
	this->initHeader();
}

/**
 * @brief Destroy the {@link TesLight::TupFile} instance and free resources.
 */
TesLight::TupFile::~TupFile()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Destroy TUP File and free resources."));
	this->close();
}

/**
 * @brief Open a TUP file and verify it.
 * @param fileSystem file system where the file is located
 * @param fileName name of the file to open
 * @return true when the file is opened
 * @return false when the file could not be opened
 */
bool TesLight::TupFile::load(FS *fileSystem, const String fileName)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Loading TUP file."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Freeing old resources."));
	this->close();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Open TUP file."));
	this->file = fileSystem->open(fileName, FILE_READ);
	if (!this->file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open TUP file. The file was not found."));
		return false;
	}
	else if (this->file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open TUP file. It is a directory."));
		this->close();
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Loading TUP file header."));
	if (!this->loadTupHeader())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The TUP file header is invalid."));
		this->close();
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("TUP file header verified."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Verifying TUP file content."));
	if (!this->verify())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to verify TUP file. The file is invalid or corrupted."));
		this->close();
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("The TUP file is valid and was loaded successfully."));

	return true;
}

/**
 * @brief Unpack the content of the valid package to a specified folder on the file system.
 * @param fileSystem file system to which to data is unpacked
 * @param root root folder to which the data is unpacked
 * @return true when the unpacking was successful
 * @return false when there was an error unpacking the data
 */
bool TesLight::TupFile::unpack(FS *fileSystem, const String root)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Unpacking TUP file to file system."));
	if (!this->file.seek(13))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to unpack TUP file. There is no content."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Unpacking data blocks to file system."));
	TesLight::TupFile::TupDataBlock dataBlock;
	dataBlock.type = TesLight::TupFile::TupDataType::FIRMWARE;
	dataBlock.path = new char[256];
	for (uint32_t i = 0; i < this->tupHeader.numberBlocks; i++)
	{
		this->file.read((uint8_t *)&dataBlock.type, 1);
		this->file.read((uint8_t *)&dataBlock.pathLength, 2);
		if (dataBlock.pathLength > 255)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("Invalid name for data block ") + String(i) + F(". Can not continue."));
			delete[] dataBlock.path;
			return false;
		}
		this->file.read((uint8_t *)dataBlock.path, dataBlock.pathLength);
		this->file.read((uint8_t *)&dataBlock.size, 4);

		const String absolutePath = this->createAbsolutePath(root + F("/"), dataBlock.path, dataBlock.pathLength);

		if (dataBlock.type == TesLight::TupFile::TupDataType::DIRECTORY)
		{
			if (!fileSystem->mkdir(absolutePath))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("Failed to create directory \"") + absolutePath + F("\". Can not continue."));
				delete[] dataBlock.path;
				return false;
			}
		}
		else if (dataBlock.type == TesLight::TupFile::TupDataType::FILE || dataBlock.type == TesLight::TupFile::TupDataType::FIRMWARE)
		{
			File file = fileSystem->open(absolutePath, FILE_WRITE);
			if (!file)
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("Failed to write file \"") + absolutePath + F("\". Can not continue."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("TUP file unpacked successfully."));
	delete[] dataBlock.path;
	return true;
}

/**
 * @brief Close the TUP file when one is opened.
 */
void TesLight::TupFile::close()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Close file and free resources."));
	this->initHeader();
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Get the currently loaded {@link TesLight::TupFile::TupHeader}.
 * @return TesLight::TupFile::TupHeader header of the loaded file
 */
TesLight::TupFile::TupHeader TesLight::TupFile::getHeader()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get TUP header."));
	return this->tupHeader;
}

/**
 * @brief Initialize the TUP header.
 */
void TesLight::TupFile::initHeader()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize TUP header."));
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
bool TesLight::TupFile::loadTupHeader()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Loading TUP header."));
	if (!this->file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Could not load TUP header because the file could not be opened."));
		return false;
	}
	else if (this->file.size() < 13)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Could not load TUP header. The file is invalid."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading header data."));
	this->file.read((uint8_t *)this->tupHeader.magic, 4);
	this->file.read((uint8_t *)&this->tupHeader.fileVersion, 1);
	this->file.read((uint8_t *)&this->tupHeader.hash, 4);
	this->file.read((uint8_t *)&this->tupHeader.numberBlocks, 4);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking magic."));
	if (this->tupHeader.magic[0] != 'T' || this->tupHeader.magic[1] != 'L' || this->tupHeader.magic[2] != 'U' || this->tupHeader.magic[3] != 'P')
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load TUP header. Magic numbers are invalid."));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Magic is valid."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking TUP file version."));
	if (this->tupHeader.fileVersion != 1)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load TUP header. Invalid file version."));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("TUP file version is valid."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking number of data blocks."));
	if (this->tupHeader.numberBlocks == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The number of data blocks is invalid. It must not be null."));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("The number of data blocks is valid."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("TUP file header loaded. The header is valid."));
	return true;
}

/**
 * @brief Verify a {@link TesLight::TupFile}.
 * @return true when the file is valid
 * @return false when the file is invalid
 */
bool TesLight::TupFile::verify()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Verifying TUP file by comparing hashes."));
	return this->generateHash() == this->tupHeader.hash;
}

/**
 * @brief Generate the simple hash to verify the TUP file.
 * @return uint32_t hash
 */
uint32_t TesLight::TupFile::generateHash()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Generate hash for TUP file."));
	if (!this->file.seek(13))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to generate hash for TUP file. There is no content."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculate hash."));
	TesLight::TupFile::TupDataBlock dataBlock;
	dataBlock.type = TesLight::TupFile::TupDataType::FIRMWARE;
	dataBlock.path = new char[256];
	uint32_t hash = 7;
	for (uint32_t i = 0; i < this->tupHeader.numberBlocks; i++)
	{
		this->file.read((uint8_t *)&dataBlock.type, 1);
		this->file.read((uint8_t *)&dataBlock.pathLength, 2);
		if (dataBlock.pathLength > 255)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The name of the data block is invalid."));
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Hash calculated."));

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
String TesLight::TupFile::createAbsolutePath(const String root, const char *name, uint16_t nameLength)
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
