/**
 * @file FileUtil.h
 * @author TheRealKasumi
 * @brief Class with general helper function to deal with files and folders.
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
#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <FS.h>
#include <functional>
#include "configuration/SystemConfiguration.h"

namespace NL
{
	class FileUtil
	{
	public:
		static bool fileExists(FS *fileSystem, const String fileName);
		static bool directoryExists(FS *fileSystem, const String path);
		static bool getFileIdentifier(FS *fileSystem, const String fileName, uint32_t &identifier);
		static bool countFiles(FS *fileSystem, const String directory, uint16_t &count, const bool includeDirs);
		static bool listFiles(FS *fileSystem, const String directory, std::function<void(const String fileName, const size_t fileSize)> callback, const bool includeDirs);
		static bool getFileNameFromIndex(FS *fileSystem, const String directory, const uint16_t fileIndex, String &fileName, const bool includeDirs);
		static bool getFileNameFromIdentifier(FS *fileSystem, const String directory, const uint32_t identifier, String &fileName);
		static bool deleteDirectory(FS *fileSystem, const String directory, const bool removeDir);
		static bool clearRoot(FS *filesSystem);

	private:
		FileUtil();
	};
}

#endif
