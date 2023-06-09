/**
 * @file FseqLoader.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::FseqLoader}.
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
#include "util/FseqLoader.h"

/**
 * @brief Create a new instance of {@link NL::FseqLoader::FseqLoader}
 * @param fileSystem file system from which the file should be loaded
 */
NL::FseqLoader::FseqLoader(FS *fileSystem)
{
	this->fileSystem = fileSystem;
	this->initFseqHeader();
	this->fillerBytes = 0;
	this->zoneCount = 1;
	this->zoneCounter = 0;
}

/**
 * @brief Destroy the {@link NL::FseqLoader::FseqLoader} and close resources.
 */
NL::FseqLoader::~FseqLoader()
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
 */
NL::FseqLoader::Error NL::FseqLoader::loadFromFile(const String fileName)
{
	this->file = this->fileSystem->open(fileName, FILE_READ);
	if (!this->file)
	{
		return NL::FseqLoader::Error::ERROR_FILE_NOT_FOUND;
	}
	else if (this->file.isDirectory())
	{
		this->file.close();
		return NL::FseqLoader::Error::ERROR_FILE_IS_DIR;
	}
	else if (this->file.size() < 28)
	{
		this->file.close();
		return NL::FseqLoader::Error::ERROR_FILE_TOO_SMALL;
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
		return NL::FseqLoader::Error::ERROR_FILE_READ;
	}

	const NL::FseqLoader::Error validationError = this->isValid();
	if (validationError != NL::FseqLoader::Error::OK)
	{
		this->file.close();
		return validationError;
	}

	this->moveToStart();
	return NL::FseqLoader::Error::OK;
}

/**
 * @brief Return the number of remaining pixels in the file.
 * @return size_t number of pixels available to read
 */
size_t NL::FseqLoader::available()
{
	return this->file ? this->file.available() / 3 : 0;
}

/**
 * @brief Reset the animation to the start.
 */
void NL::FseqLoader::moveToStart()
{
	if (this->file)
	{
		this->file.seek(this->fseqHeader.channelDataOffset);
		this->zoneCounter = 0;
	}
}

/**
 * @brief Close the input file.
 */
void NL::FseqLoader::close()
{
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Get the file header.
 * @return {@link NL::FseqLoader::FseqHeader} header of the opened file
 */
NL::FseqLoader::FseqHeader NL::FseqLoader::getHeader()
{
	return this->fseqHeader;
}

/**
 * @brief Read a LED strip from the stream.
 * @param ledStrip LED strip with the pixel data
 * @return OK when the pixel buffer was read
 * @return ERROR_END_OF_FILE when there is no more data to read
 */
NL::FseqLoader::Error NL::FseqLoader::readLedStrip(NL::LedStrip &ledStrip)
{
	if (this->file && this->available() >= ledStrip.getLedCount())
	{
		this->zoneCounter++;
		if (this->zoneCounter >= this->zoneCount)
		{
			this->zoneCounter = 0;
			if (!this->file.seek(this->fillerBytes, fs::SeekMode::SeekCur))
			{
				return NL::FseqLoader::Error::ERROR_END_OF_FILE;
			}
		}

		if (this->file.read(ledStrip.getBuffer(), ledStrip.getLedCount() * 3) == ledStrip.getLedCount() * 3)
		{
			return NL::FseqLoader::Error::OK;
		}
	}

	return NL::FseqLoader::Error::ERROR_END_OF_FILE;
}

/**
 * @brief Set the number of filler bytes which is skipped after each completed frame.
 * @param fillerBytes number of filler bytes
 */
void NL::FseqLoader::setFillerBytes(const uint8_t fillerBytes)
{
	this->fillerBytes = fillerBytes;
}

/**
 * @brief Get the number of filler bytes which is skipped after each completed frame.
 */
uint8_t NL::FseqLoader::getFillerBytes()
{
	return this->fillerBytes;
}

/**
 * @brief Set the number of zones that will read from the file.
 * @param zoneCount number of zones
 */
void NL::FseqLoader::setZoneCount(const uint8_t zoneCount)
{
	this->zoneCount = zoneCount;
}

/**
 * @brief Get the number of zones that will read from the file.
 */
uint8_t NL::FseqLoader::getZoneCount()
{
	return this->zoneCount;
}

/**
 * @brief Initialize the fseqHeader with 0.
 */
void NL::FseqLoader::initFseqHeader()
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
 */
NL::FseqLoader::Error NL::FseqLoader::isValid()
{
	// Check the identifier
	if (this->fseqHeader.identifier[0] != 'P' || this->fseqHeader.identifier[1] != 'S' || this->fseqHeader.identifier[2] != 'E' || this->fseqHeader.identifier[3] != 'Q')
	{
		return NL::FseqLoader::Error::ERROR_MAGIC_NUMBERS;
	}

	// Check the version
	if (this->fseqHeader.minorVersion != 0 || this->fseqHeader.majorVersion != 1)
	{
		return NL::FseqLoader::Error::ERROR_FILE_VERSION;
	}

	// Check the header length
	if (this->fseqHeader.headerLength != 28)
	{
		return NL::FseqLoader::Error::ERROR_HEADER_LENGTH;
	}

	// Check the channelCount, frameCount and data block length
	uint32_t dataLength = this->file.size() - this->fseqHeader.channelDataOffset;
	uint32_t expectedLength = this->fseqHeader.channelCount * this->fseqHeader.frameCount;
	if (dataLength != expectedLength)
	{
		return NL::FseqLoader::Error::ERROR_INVALID_DATA_LENGTH;
	}

	return NL::FseqLoader::Error::OK;
}
