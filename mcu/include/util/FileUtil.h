/**
 * @file FileUtil.h
 * @author TheRealKasumi
 * @brief Class with general helper function to deal with files and folders.
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
