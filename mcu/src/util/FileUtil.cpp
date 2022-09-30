/**
 * @file FileUtil.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::FileUtil}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "util/FileUtil.h"

/**
 * @brief Get a list of files in a directory.
 * @param fileSystem where to look for the files
 * @param directory containing the files
 * @param fileList reference to a string that will store the file list
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::FileUtil::getFileList(FS *fileSystem, const String directory, String &fileList)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("List all files from directory ") + directory + F("."));
	fileList = String();

	File dir = fileSystem->open(directory, FILE_READ);
	if (!dir)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open directory. Directory could not be found."));
		return false;
	}
	else if (!dir.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open directory. It is a file."));
		dir.close();
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Iterate over files."));
	File file;
	bool hasNext = true;
	while (hasNext)
	{
		file = dir.openNextFile(FILE_READ);
		if (file)
		{
			if (!file.isDirectory())
			{
				fileList += String(file.size()) + F(";");
				fileList += String(file.name()) + F("\n");
			}
			file.close();
		}
		else
		{
			hasNext = false;
		}
	}

	dir.close();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Remove new-line at the end of the list."));
	if (fileList.length() > 0)
	{
		fileList = fileList.substring(0, fileList.length() - 1);
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("All files listed successfully."));
	return true;
}

/**
 * @brief Get a file name by the file index.
 * @param fileSystem where to look for the files
 * @param directory containing the files
 * @param fileIndex index of the file in the file system
 * @param fileName name of the file with the searched index of empty string when nothing found
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::FileUtil::getFileNameByIndex(FS *fileSystem, const String directory, const uint8_t fileIndex, String &fileName)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Get filename of file with index ") + String(fileIndex) + F(" from directory ") + directory + F("."));

	File dir = fileSystem->open(directory, FILE_READ);
	if (!dir)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open directory. Directory could not be found."));
		return false;
	}
	else if (!dir.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open directory. It is a file."));
		dir.close();
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Iterate over files."));
	File file;
	bool hasNext = true;
	uint8_t currentIndex = 0;
	while (hasNext)
	{
		file = dir.openNextFile(FILE_READ);
		if (file)
		{
			if (!file.isDirectory())
			{
				if (currentIndex == fileIndex)
				{
					TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("File found: ") + file.name());
					fileName = file.name();
					hasNext = false;
				}
				currentIndex++;
			}
			file.close();
		}
		else
		{
			fileName = String();
			hasNext = false;
		}
	}

	dir.close();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("File search successful."));
	return true;
}