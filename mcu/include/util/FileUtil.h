/**
 * @file FileUtil.h
 * @author TheRealKasumi
 * @brief Class with general helper function to deal with files and folders.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <FS.h>
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"

namespace TesLight
{
	class FileUtil
	{
	public:
		static bool fileExists(FS *fileSystem, const String fileName);
		static bool directoryExists(FS *fileSystem, const String path);

		static bool countFiles(FS *fileSystem, const String directory, uint16_t &count, const bool includeDirs);
		static bool getFileList(FS *fileSystem, const String directory, String &fileList, const bool includeDirs);
		static bool getFileNameFromIndex(FS *fileSystem, const String directory, const uint16_t fileIndex, String &fileName, const bool includeDirs);

		static bool deleteDirectory(FS *fileSystem, const String directory, const bool removeDir);
		static bool clearRoot(FS *filesSystem);

	private:
		FileUtil();
	};
}

#endif