/**
 * @file NUPFile.h
 * @author TheRealKasumi
 * @brief Contains a class for building a NikoLight Update Package file from a folder.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef NUP_FILE_H
#define NUP_FILE_H

#include <stdint.h>
#include <cstring>
#include <vector>
#include <queue>
#include <filesystem>
#include <fstream>

class NUPFile
{
public:
	struct NUPHeader
	{
		char magic[4];
		uint8_t fileVersion;
		uint32_t hash;
		uint32_t numberBlocks;
	};

	enum NUPDataType
	{
		FIRMWARE = 0,
		FILE = 1,
		DIRECTORY = 2
	};

	struct NUPDataBlock
	{
		NUPDataType type;
		uint16_t pathLength;
		char *path;
		uint32_t size;
		uint8_t *data;
	};

	NUPFile();
	~NUPFile();

	bool generateFromFolder(const std::filesystem::path rootPath);
	bool saveToFile(const std::filesystem::path fileName);

private:
	NUPHeader header;
	std::vector<NUPDataBlock> dataBlocks;

	void addFolder(const std::filesystem::path path);
	bool addFile(const std::filesystem::path fileName, const std::filesystem::path name);

	uint32_t generateHash();
};

#endif