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
#include "logging/Logger.h"

namespace TesLight
{
	class FileUtil
	{
	public:
		static bool getFileList(FS *fileSystem, const String directory, String &fileList);
		static bool getFileNameByIndex(FS *fileSystem, const String directory, const uint8_t fileIndex, String &fileName);

	private:
		FileUtil();
	};
}

#endif