/**
 * @file NUPFile.h
 * @author TheRealKasumi
 * @brief Contains a class for building a NikoLight Update Package file from a folder.
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