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
 * @brief Check if a file exists on the file system.
 * @param fileSystem where the file is stored
 * @param fileName full path and name of the file
 * @return true when the file exists
 * @return false when the file does not exist
 */
bool TesLight::FileUtil::fileExists(FS *fileSystem, const String fileName)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Checking if file \"") + fileName + F("\" exists."));
	File file = fileSystem->open(fileName, FILE_READ);
	if (!file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Failed to open file."));
		return false;
	}
	else if (file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Failed to open file. It is a directory."));
		file.close();
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("File was found."));
	file.close();
	return true;
}

/**
 * @brief Check if a directory exists on the file system.
 * @param fileSystem where the directory is stored
 * @param path full path and name of the directory
 * @return true when the directory exists
 * @return false when the directory does not exist
 */
bool TesLight::FileUtil::directoryExists(FS *fileSystem, const String path)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Checking if directory \"") + path + F("\" exists."));
	File file = fileSystem->open(path, FILE_READ);
	if (!file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Failed to open directory."));
		return false;
	}
	else if (!file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Failed to open directory. It is a file."));
		file.close();
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Directory was found."));
	file.close();
	return true;
}

/**
 * @brief Count the number of files and optinally directories inside a directory.
 * @param fileSystem where the root directory is located
 * @param directory full path and name to the root directory
 * @param count reference variable, number of counted files and directories will be written here
 * @param includeDirs true to include directories, false to only count files
 * @return true when the counting was successful
 * @return false when there was an error counting
 */
bool TesLight::FileUtil::countFiles(FS *fileSystem, const String directory, uint16_t &count, const bool includeDirs)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Count the number of files (and directories) in \"") + directory + F("\"."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Counting files and folders."));
	count = 0;
	bool hasNext = true;
	while (hasNext)
	{
		File file = dir.openNextFile(FILE_READ);
		if (file)
		{
			if (includeDirs || !file.isDirectory())
			{
				count++;
			}
			file.close();
		}
		else
		{
			hasNext = false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("File counting successful."));
	dir.close();
	return true;
}

/**
 * @brief Get a list of all files, optinally directories and their file size in bytes. The list is seperated by newline.
 * @param fileSystem where the root directory is located
 * @param directory root directory for the search
 * @param fileList reference variable, the list is stored here
 * @param includeDirs true to include directories, false to only list files
 * @return true when the list was created successfully
 * @return false when there was an error
 */
bool TesLight::FileUtil::getFileList(FS *fileSystem, const String directory, String &fileList, const bool includeDirs)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("List all files (and directories) from root directory ") + directory + F("."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Listing files (and directories)."));
	fileList = String();
	bool hasNext = true;
	while (hasNext)
	{
		File file = dir.openNextFile(FILE_READ);
		if (file)
		{
			if (includeDirs || !file.isDirectory())
			{
				fileList += String(file.name()) + F(";");
				fileList += String(file.size()) + F("\n");
			}
			file.close();
		}
		else
		{
			hasNext = false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("All files listed successfully."));
	dir.close();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Remove new-line at the end of the list."));
	if (fileList.length() > 0)
	{
		fileList = fileList.substring(0, fileList.length() - 1);
	}
	return true;
}

/**
 * @brief Get the name of a file or directory based on it's index in the file system.
 * @param fileSystem where the file is located
 * @param directory root directory for the file or directory search
 * @param fileIndex index of the file or directory in the file system
 * @param fileName reference to the full file or directory name
 * @param includeDirs true to include directories, false to only include files
 * @return true when the seach was successful
 * @return false when there was an error
 */
bool TesLight::FileUtil::getFileNameFromIndex(FS *fileSystem, const String directory, const uint16_t fileIndex, String &fileName, const bool includeDirs)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Get name of file (or directory) with index ") + String(fileIndex) + F(" in root directory ") + directory + F("."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Search for file or directory."));
	uint16_t currentIndex = 0;
	bool hasNext = true;
	while (hasNext)
	{
		File file = dir.openNextFile(FILE_READ);
		if (file)
		{
			if (includeDirs || !file.isDirectory())
			{
				if (currentIndex == fileIndex)
				{
					TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("File or directory was found at \"") + directory + F("/") + file.name() + F("\"."));
					fileName = file.name();
					hasNext = false;
				}
				currentIndex++;
			}
			file.close();
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("No matching file or directory found."));
			fileName = String();
			hasNext = false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("File search successful."));
	dir.close();
	return true;
}

/**
 * @brief Recursively delete a directory with all it's contents.
 * @param fileSystem where the root directory is located
 * @param directory full path and name to the directory that should be deleted
 * @param removeDir true to remove the empty root directory, false to keep it
 * @return true when the directory was deleted
 * @return false when there was an error
 */
bool TesLight::FileUtil::deleteDirectory(FS *fileSystem, const String directory, const bool removeDir)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Deleting directory \"") + directory + F("\"."));
	uint16_t fileCount = 0;
	if (!TesLight::FileUtil::countFiles(fileSystem, directory, fileCount, true))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to count files in the directory that should be deleted."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Iterating over ") + fileCount + F(" files and directories for deletion."));
	for (uint16_t i = 0; i < fileCount; i++)
	{
		String name;
		if (!TesLight::FileUtil::getFileNameFromIndex(fileSystem, directory, i, name, true))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to get next file or directory."));
			return false;
		}
		name = directory == F("/") ? (String)F("/") + name : directory + F("/") + name;

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Check if \"") + name + F("\" is excluded from the deletion."));
		if (name == LOG_FILE_NAME || name == CONFIGURATION_FILE_NAME || name == FSEQ_DIRECTORY || name == UPDATE_DIRECTORY)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("It is excluded, continuing with next entity."));
			continue;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Check if \"") + name + F("\" is a file or directory."));
		if (TesLight::FileUtil::directoryExists(fileSystem, name))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("It is a directory, recursively diving into it for deletion."));
			if (!TesLight::FileUtil::deleteDirectory(fileSystem, name, true))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to recursively delete directory \"") + name + F("\"."));
				return false;
			}
			i--;
			fileCount--;
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Directory deleted."));
		}
		else if (TesLight::FileUtil::fileExists(fileSystem, name))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("It is a file, continuing with deletion."));
			if (!fileSystem->remove(name))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to delete file \"") + name + F("\"."));
				return false;
			}
			i--;
			fileCount--;
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("File deleted."));
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("It is not a file but also no directory. Something is wrong."));
			return false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Directory \"") + directory + F("\" is cleared."));
	if (removeDir)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Removing directory \"") + directory + F("\"."));
		if (!fileSystem->rmdir(directory))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to remove directory."));
			return false;
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Directory was removed."));
			return true;
		}
	}
	else
	{
		return true;
	}
}

/**
 * @brief Clear the root directory of a file system.
 * @param filesSystem which root should be cleared
 * @return true when cleared successfully
 * @return false when there was an error
 */
bool TesLight::FileUtil::clearRoot(FS *filesSystem)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Clearing root directory."));
	if (TesLight::FileUtil::deleteDirectory(filesSystem, F("/"), false))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Root directory was cleared."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Failed to clear root directory."));
		return false;
	}
}
