/**
 * @file TUPFile.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TUPFile} class.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "TUPFile.h"

/**
 * @brief Create a new instance of {@link TUPFile}.
 */
TUPFile::TUPFile()
{
	this->header.magic[0] = 'T';
	this->header.magic[0] = 'L';
	this->header.magic[0] = 'U';
	this->header.magic[0] = 'P';
	this->header.fileVersion = 1;
	this->header.hash = 0;
}

/**
 * @brief Destroy the {@link TUPFile} instance and free memory.
 */
TUPFile::~TUPFile()
{
	for (size_t i = 0; i < this->dataBlocks.size(); i++)
	{
		const TUPDataBlock dataBlock = this->dataBlocks[i];
		if (dataBlock.path != nullptr)
		{
			delete[] dataBlock.path;
		}
		if (dataBlock.data != nullptr)
		{
			delete[] dataBlock.data;
		}
	}
}

/**
 * @brief Generate the TUP file based on the given folder.
 * @param rootPath root path of the folder with all update files
 * @return true when the file was generated successfully (in memory)
 * @return false when there was an error constructing the TUP
 */
bool TUPFile::generateFromFolder(const std::filesystem::path rootPath)
{
	std::queue<std::filesystem::path> queue;
	queue.push("");

	while (!queue.empty())
	{
		const std::filesystem::path relativePath = queue.front();
		const std::filesystem::path absolutePath = rootPath / relativePath;
		queue.pop();

		for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(absolutePath))
		{
			if (entry.is_directory())
			{
				const std::filesystem::path path = relativePath / entry.path().filename();
				this->addFolder(path);
				queue.push(path);
			}
			else if (entry.is_regular_file())
			{
				const std::filesystem::path name = relativePath / entry.path().filename();
				if (!this->addFile(entry.path(), name))
				{
					return false;
				}
			}
		}
	}

	return true;
}

/**
 * @brief Save the in memory TUP file to a file on the disk.
 * @param fileName output file name for the TUP file
 * @return true when the file was written successfully
 * @return false when there was an error writing the file
 */
bool TUPFile::saveToFile(const std::filesystem::path fileName)
{
	std::ofstream file(fileName, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	TUPHeader header;
	header.magic[0] = 'T';
	header.magic[1] = 'L';
	header.magic[2] = 'U';
	header.magic[3] = 'P';
	header.fileVersion = 1;
	header.hash = this->generateHash();
	header.numberBlocks = this->dataBlocks.size();

	file.write(header.magic, 4);
	file.write((char *)&header.fileVersion, 1);
	file.write((char *)&header.hash, 4);
	file.write((char *)&header.numberBlocks, 4);

	for (size_t i = 0; i < this->dataBlocks.size(); i++)
	{
		const TUPDataBlock dataBlock = this->dataBlocks[i];
		file.write((char *)&dataBlock.type, 1);
		file.write((char *)&dataBlock.pathLength, 2);
		file.write(dataBlock.path, dataBlock.pathLength);
		file.write((char *)&dataBlock.size, 4);
		file.write((char *)dataBlock.data, dataBlock.size);
	}

	file.close();
	return true;
}

/**
 * @brief Add a folder to the TUP file. It doesn't contain any data and is only for the folder creating on the MircoSD card.
 * @param path relative path of the folder (to the root folder)
 */
void TUPFile::addFolder(const std::filesystem::path path)
{
	TUPDataBlock dataBlock;
	dataBlock.type = TUPDataType::DIRECTORY;
	dataBlock.pathLength = path.string().length();
	dataBlock.path = new char[dataBlock.pathLength];
	std::memcpy(dataBlock.path, path.string().c_str(), dataBlock.pathLength);
	dataBlock.size = 0;
	dataBlock.data = nullptr;
	this->dataBlocks.push_back(dataBlock);
}

/**
 * @brief Add a file to the TUP file. The data of the file is loaded into memory and kept for writing it to the disk later.
 * @param fileName absolute file path and name to the file that has to be embedded.
 * @param name relative path and name of the file (to the root folder)
 * @return true when the file was embedded successfully
 * @return false when there was an error embedding the file
 */
bool TUPFile::addFile(const std::filesystem::path fileName, const std::filesystem::path name)
{
	const size_t fileSize = std::filesystem::file_size(fileName);
	if (fileSize == 0)
	{
		return false;
	}

	std::ifstream file(fileName, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	TUPDataBlock dataBlock;
	dataBlock.type = name == "firmware.bin" ? TUPDataType::FIRMWARE : TUPDataType::FILE;
	dataBlock.pathLength = name.string().length();
	dataBlock.path = new char[dataBlock.pathLength];
	std::memcpy(dataBlock.path, name.string().c_str(), dataBlock.pathLength);
	dataBlock.size = fileSize;
	dataBlock.data = new uint8_t[fileSize];

	if (!file.read((char *)dataBlock.data, fileSize))
	{
		delete[] dataBlock.path;
		delete[] dataBlock.data;
		file.close();
		return false;
	}

	file.close();
	this->dataBlocks.push_back(dataBlock);
	return true;
}

/**
 * @brief Generate the simple hash for a weak package check.
 * @return uint32_t 4 bytes with the hash
 */
uint32_t TUPFile::generateHash()
{
	uint32_t hash = 7;
	for (size_t i = 0; i < this->dataBlocks.size(); i++)
	{
		const TUPDataBlock dataBlock = this->dataBlocks[i];
		hash = hash * 31 + dataBlock.type;
		hash = hash * 31 + dataBlock.pathLength;
		for (size_t j = 0; j < dataBlock.pathLength; j++)
		{
			hash = hash * 31 + dataBlock.path[j];
		}
		hash = hash * 31 + dataBlock.size;
		for (size_t j = 0; j < dataBlock.size; j++)
		{
			hash = hash * 31 + dataBlock.data[j];
		}
	}
	return hash;
}