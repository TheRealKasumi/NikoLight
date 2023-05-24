/**
 * @file Updater.h
 * @author TheRealKasumi
 * @brief Class to install system updates from a TUP file.
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