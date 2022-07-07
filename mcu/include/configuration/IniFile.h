/**
 * @file IniFile.h
 * @author TheRealKasumi
 * @brief Contains a class to load and save ini files.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef INIFILE_H
#define INIFILE_H

#include <Arduino.h>
#include <FS.h>
#include <map>

#include "logging/Logger.h"

namespace TesLight
{
	class IniFile
	{
	public:
		IniFile(FS *fileSystem);
		~IniFile();

		bool load(const String fileName);
		bool save(const String fileName);

		bool keyExists(const String key);

		String getValue(const String key, const String defaultValue = F(""));
		void setValue(const String key, const String value);
		bool removeValue(const String key);

	private:
		FS *fileSystem;
		std::map<String, String> keyValueMap;

		bool isValid();
		bool isValidLine(const String iniFileLine);

		String extractKey(const String iniFileLine);
		String extractValue(const String iniFileLine);

		String readNextLine(File *file);
		void writeLine(File *file, const String input);
	};
}

#endif