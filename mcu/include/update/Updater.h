/**
 * @file Updater.h
 * @author TheRealKasumi
 * @brief Class to install system updates from a TUP file.
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
#ifndef UPDATER_H
#define UPDATER_H

#include <stdint.h>
#include <WString.h>
#include <FS.h>
#include <Update.h>

#include "logging/Logger.h"
#include "update/NupFile.h"
#include "util/FileUtil.h"

namespace NL
{
	class Updater
	{
	public:
		enum class Error
		{
			OK,							 // No error
			ERROR_UPDATE_FILE_NOT_FOUND, // Update file was not found
			ERROR_INVALID_FILE,			 // Update file is invalid
			ERROR_CLEAN_FS,				 // Failed to clean FS for the update
			ERROR_UPDATE_UNPACK,		 // Failed to unpack the update package
			ERROR_FW_FILE_NOT_FOUND,	 // The firmware file was not found
			ERROR_FW_FILE_EMPTY,		 // The firmware file is empty
			ERROR_OUT_OF_FLASH_MEMORY,	 // Not enough flash memory to install the firmware
			ERROR_WRITE_FW_DATA,		 // Not all firmware data was written
			ERROR_FINISH_FW_UPDATE,		 // Failed to finish the firmware update

		};

		static NL::Updater::Error install(FS *fileSystem, const String packageFileName);
		static void reboot(const String reason, const uint16_t delayMillis);

	private:
		Updater();

		static NL::Updater::Error installFirmware(FS *fileSystem, const String firmwareFileName);
		static void rebootInt(void *params);
	};
}

#endif