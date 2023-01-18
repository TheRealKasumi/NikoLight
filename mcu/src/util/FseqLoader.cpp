/**
 * @file FseqLoader.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::FseqLoader}.
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
#include "util/FseqLoader.h"

/**
 * @brief Create a new instance of {@link TL::FseqLoader::FseqLoader}
 * @param fileSystem file system from which the file should be loaded
 */
TL::FseqLoader::FseqLoader(FS *fileSystem)
{
	this->fileSystem = fileSystem;
	this->initFseqHeader();
}

/**
 * @brief Destroy the {@link TL::FseqLoader::FseqLoader} and close resources.
 */
TL::FseqLoader::~FseqLoader()
{
	this->close();
}

/**
 * @brief Load a fseq version 1.0 file from the file system and check if it's valid.
 * @param fileName full name and path of the fseq file
 * @return OK when the file was loaded and is valid
 * @return ERROR_FILE_NOT_FOUND when the file was not found
 * @return ERROR_FILE_IS_DIR when the file is a directory
 * @return ERROR_FILE_TOO_SMALL when the file is too small to be valid
 * @return ERROR_FILE_READ when the file could not be read
 * @return ERROR_MAGIC_NUMBERS when the magic numbers do not match
 * @return ERROR_FILE_VERSION  when the file version is unsupported
 * @return ERROR_HEADER_LENGTH when the header length is invalid
 * @return ERROR_INVALID_DATA_LENGTH when the data length does not match the length specified in header
 * @return ERROR_UNSUPPORTED_DATA_LENGTH when the data length is not a multiple of 3 and unsupported by TesLight
 */
TL::FseqLoader::Error TL::FseqLoader::loadFromFile(const String fileName)
{
	this->file = this->fileSystem->open(fileName, FILE_READ);
	if (!this->file)
	{
		return TL::FseqLoader::Error::ERROR_FILE_NOT_FOUND;
	}
	else if (this->file.isDirectory())
	{
		this->file.close();
		return TL::FseqLoader::Error::ERROR_FILE_IS_DIR;
	}
	else if (this->file.size() < 28)
	{
		this->file.close();
		return TL::FseqLoader::Error::ERROR_FILE_TOO_SMALL;
	}

	bool readError = false;
	this->initFseqHeader();
	readError = this->file.readBytes((char *)&this->fseqHeader.identifier[0], 4) != 4 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.channelDataOffset, 2) != 2 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.minorVersion, 1) != 1 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.majorVersion, 1) != 1 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.headerLength, 2) != 2 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.channelCount, 4) != 4 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.frameCount, 4) != 4 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.stepTime, 1) != 1 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.flags, 1) != 1 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.universeCount, 2) != 2 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.universeSize, 2) != 2 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.gamma, 1) != 1 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.colorEncoding, 1) != 1 ? true : readError;
	readError = this->file.readBytes((char *)&this->fseqHeader.reserved, 2) != 2 ? true : readError;
	if (readError)
	{
		this->file.close();
		return TL::FseqLoader::Error::ERROR_FILE_READ;
	}

	const TL::FseqLoader::Error validationError = this->isValid();
	if (validationError != TL::FseqLoader::Error::OK)
	{
		this->file.close();
		return validationError;
	}

	this->moveToStart();
	return TL::FseqLoader::Error::OK;
}

/**
 * @brief Return the number of remaining pixels in the file.
 * @return size_t number of pixels available to read
 */
size_t TL::FseqLoader::available()
{
	return this->file ? this->file.available() / 3 : 0;
}

/**
 * @brief Reset the animation to the start.
 */
void TL::FseqLoader::moveToStart()
{
	if (this->file)
	{
		this->file.seek(this->fseqHeader.channelDataOffset);
	}
}

/**
 * @brief Close the input file.
 */
void TL::FseqLoader::close()
{
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Get the file header.
 * @return {@link TL::FseqLoader::FseqHeader} header of the opened file
 */
TL::FseqLoader::FseqHeader TL::FseqLoader::getHeader()
{
	return this->fseqHeader;
}

/**
 * @brief Read a buffer of pixels from the stream.
 * @param pixels reference to the vector holding the LED pixel data
 * @return OK when the pixel buffer was read
 * @return ERROR_END_OF_FILE when there is no more data to read
 */
TL::FseqLoader::Error TL::FseqLoader::readPixelBuffer(std::vector<CRGB> &pixels)
{
	if (this->file && this->available() >= pixels.size())
	{
		if (this->file.read((uint8_t *)&pixels.front(), pixels.size() * 3) == pixels.size() * 3)
		{
			return TL::FseqLoader::Error::OK;
		}
	}

	return TL::FseqLoader::Error::ERROR_END_OF_FILE;
}

/**
 * @brief Initialize the fseqHeader with 0.
 */
void TL::FseqLoader::initFseqHeader()
{
	this->fseqHeader.identifier[0] = 0;
	this->fseqHeader.identifier[1] = 0;
	this->fseqHeader.identifier[2] = 0;
	this->fseqHeader.identifier[3] = 0;
	this->fseqHeader.channelDataOffset = 0;
	this->fseqHeader.minorVersion = 0;
	this->fseqHeader.majorVersion = 0;
	this->fseqHeader.headerLength = 0;
	this->fseqHeader.channelCount = 0;
	this->fseqHeader.frameCount = 0;
	this->fseqHeader.stepTime = 0;
	this->fseqHeader.flags = 0;
	this->fseqHeader.universeCount = 0;
	this->fseqHeader.universeSize = 0;
	this->fseqHeader.gamma = 0;
	this->fseqHeader.colorEncoding = 0;
	this->fseqHeader.reserved = 0;
}

/**
 * @brief Check if the opened fseq file is valid.
 * @return OK when the file is valid
 * @return ERROR_MAGIC_NUMBERS when the magic numbers do not match
 * @return ERROR_FILE_VERSION  when the file version is unsupported
 * @return ERROR_HEADER_LENGTH when the header length is invalid
 * @return ERROR_INVALID_DATA_LENGTH when the data length does not match the length specified in header
 * @return ERROR_UNSUPPORTED_DATA_LENGTH when the data length is not a multiple of 3 and unsupported by TesLight
 */
TL::FseqLoader::Error TL::FseqLoader::isValid()
{
	// Check the identifier
	if (this->fseqHeader.identifier[0] != 'P' || this->fseqHeader.identifier[1] != 'S' || this->fseqHeader.identifier[2] != 'E' || this->fseqHeader.identifier[3] != 'Q')
	{
		return TL::FseqLoader::Error::ERROR_MAGIC_NUMBERS;
	}

	// Check the version
	if (this->fseqHeader.minorVersion != 0 || this->fseqHeader.majorVersion != 1)
	{
		return TL::FseqLoader::Error::ERROR_FILE_VERSION;
	}

	// Check the header length
	if (this->fseqHeader.headerLength != 28)
	{
		return TL::FseqLoader::Error::ERROR_HEADER_LENGTH;
	}

	// Check the channelCount, frameCount and data block length
	uint32_t dataLength = this->file.size() - this->fseqHeader.channelDataOffset;
	uint32_t expectedLength = this->fseqHeader.channelCount * this->fseqHeader.frameCount;
	if (dataLength != expectedLength)
	{
		return TL::FseqLoader::Error::ERROR_INVALID_DATA_LENGTH;
	}

	// Check if the data length is a multiple of 3
	// It has to be a multiple of 3 because there are 3 bytes per pixel
	if (dataLength % 3 != 0)
	{
		return TL::FseqLoader::Error::ERROR_UNSUPPORTED_DATA_LENGTH;
	}

	return TL::FseqLoader::Error::OK;
}
