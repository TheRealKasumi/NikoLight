/**
 * @file Updater.h
 * @author TheRealKasumi
 * @brief Class to install system updates from a TUP file.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef UPDATER_H
#define UPDATER_H

#include <Arduino.h>
#include <FS.h>
#include <Update.h>
#include "logging/Logger.h"
#include "update/TupFile.h"
#include "util/FileUtil.h"

namespace TesLight
{
	class Updater
	{
	public:
		static bool install(FS *fileSystem, const String packageFileName);
		static void reboot(const String reason);

	private:
		Updater();

		static bool installFirmware(FS *fileSystem, const String firmwareFileName);
	};
}

#endif