/**
 * @file FseqLoader.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::FseqLoader}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "util/FseqLoader.h"

/**
 * @brief Create a new instance of {@link TesLight::FseqLoader::FseqLoader}
 * @param fileSystem file system from which the file should be loaded
 */
TesLight::FseqLoader::FseqLoader(FS *fileSystem)
{
	this->fileSystem = fileSystem;
}

/**
 * @brief Destroy the {@link TesLight::FseqLoader::FseqLoader} and close resources.
 */
TesLight::FseqLoader::~FseqLoader()
{
	this->close();
}

/**
 * @brief Load a fseq version 1.0 file from the file system and check if it's valid.
 * @param fileName full name and path of the fseq file
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::FseqLoader::loadFromFile(const String fileName)
{
	this->file = this->fileSystem->open(fileName, FILE_READ);
	if (!this->file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open fseq file."));
		return false;
	}
	else if (this->file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open fseq file because it is a directory."));
		this->file.close();
		return false;
	}
	else if (this->file.size() < 28)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open fseq file because it is too small. A valid fseq file must have at least 28 bytes."));
		this->file.close();
		return false;
	}

	this->initFseqHeader();
	this->file.readBytes((char *)&this->fseqHeader.identifier[0], 4);
	this->file.readBytes((char *)&this->fseqHeader.channelDataOffset, 2);
	this->file.readBytes((char *)&this->fseqHeader.minorVersion, 1);
	this->file.readBytes((char *)&this->fseqHeader.majorVersion, 1);
	this->file.readBytes((char *)&this->fseqHeader.headerLength, 2);
	this->file.readBytes((char *)&this->fseqHeader.channelCount, 4);
	this->file.readBytes((char *)&this->fseqHeader.frameCount, 4);
	this->file.readBytes((char *)&this->fseqHeader.stepTime, 1);
	this->file.readBytes((char *)&this->fseqHeader.flags, 1);
	this->file.readBytes((char *)&this->fseqHeader.universeCount, 2);
	this->file.readBytes((char *)&this->fseqHeader.universeSize, 2);
	this->file.readBytes((char *)&this->fseqHeader.gamma, 1);
	this->file.readBytes((char *)&this->fseqHeader.colorEncoding, 1);
	this->file.readBytes((char *)&this->fseqHeader.reserved, 2);

	if (this->isValid())
	{
		this->moveToStart();
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("File is invalid."));
		this->file.close();
		return false;
	}
}

/**
 * @brief Return the number of remaining pixels in the file.
 * @return size_t number of pixels available to read
 */
size_t TesLight::FseqLoader::available()
{
	return this->file ? this->file.available() / 3 : 0;
}

/**
 * @brief Reset the animation to the start.
 */
void TesLight::FseqLoader::moveToStart()
{
	if (this->file)
	{
		this->file.seek(this->fseqHeader.channelDataOffset);
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can't move to the start of the data section because the file is not opened."));
	}
}

/**
 * @brief Close the input file.
 */
void TesLight::FseqLoader::close()
{
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Get the file header.
 * @return {@link TesLight::FseqLoader::FseqHeader} header of the opened file
 */
TesLight::FseqLoader::FseqHeader TesLight::FseqLoader::getHeader()
{
	return this->fseqHeader;
}

/**
 * @brief Read a buffer of pixels from the stream.
 * @param pixelBuffer pointer to a {@link CRGB buffer} for the pixel data
 * @param bufferSize number of pixels in the buffer/number of pixels to be read from the stream
 * @return true when read successfully
 * @return false when there was an error
 */
bool TesLight::FseqLoader::readPixelbuffer(CRGB *pixelBuffer, const size_t bufferSize)
{
	if (this->file && this->available() >= bufferSize)
	{
		if (this->file.read((uint8_t *)pixelBuffer, bufferSize * 3) == bufferSize * 3)
		{
			return true;
		}
	}

	return false;
}

/**
 * @brief Initialize the fseqHeader with 0.
 */
void TesLight::FseqLoader::initFseqHeader()
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
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::FseqLoader::isValid()
{
	// Check the identifier
	if (this->fseqHeader.identifier[0] != 'P' || this->fseqHeader.identifier[1] != 'S' || this->fseqHeader.identifier[2] != 'E' || this->fseqHeader.identifier[3] != 'Q')
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file is invalid because the identifier doesn't match. It must be PSEQ."));
		return false;
	}

	// Check the version
	if (this->fseqHeader.minorVersion != 0 || this->fseqHeader.majorVersion != 1)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file is invalid because the version does not match. It must be 1.0."));
		return false;
	}

	// Check the header length
	if (this->fseqHeader.headerLength != 28)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file is invalid because the header length is not valid."));
		return false;
	}

	// Check the channelCount, frameCount and data block length
	uint32_t dataLength = this->file.size() - this->fseqHeader.channelDataOffset;
	uint32_t expectedLength = this->fseqHeader.channelCount * this->fseqHeader.frameCount;
	if (dataLength != expectedLength)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file is invalid because the data length does not match den length defined in the header."));
		return false;
	}

	// Check if the data length is a multiple of 3
	// It has to be a multiple of 3 because there are 3 bytes per pixel
	if (dataLength % 3 != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file is invalid because the channel count is not a multiple of 3. This is a requirement by TesLight."));
		return false;
	}

	return true;
}
