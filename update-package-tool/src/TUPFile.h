/**
 * @file TUPFile.h
 * @author TheRealKasumi
 * @brief Contains a class for building a TesLight Update Package file from a folder.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef TUP_FILE_H
#define TUP_FILE_H

#include <stdint.h>
#include <vector>
#include <queue>
#include <filesystem>
#include <fstream>

class TUPFile
{
public:
	struct TUPHeader
	{
		char magic[4];
		uint8_t fileVersion;
		uint32_t hash;
		uint32_t numberBlocks;
	};

	enum TUPDataType
	{
		FIRMWARE = 0,
		FILE = 1,
		DIRECTORY = 2
	};

	struct TUPDataBlock
	{
		TUPDataType type;
		uint16_t pathLength;
		char *path;
		uint32_t size;
		uint8_t *data;
	};

	TUPFile();
	~TUPFile();

	bool generateFromFolder(const std::filesystem::path rootPath);
	bool saveToFile(const std::filesystem::path fileName);

private:
	TUPHeader header;
	std::vector<TUPDataBlock> dataBlocks;

	void addFolder(const std::filesystem::path path);
	bool addFile(const std::filesystem::path fileName, const std::filesystem::path name);

	uint32_t generateHash();
};

#endif