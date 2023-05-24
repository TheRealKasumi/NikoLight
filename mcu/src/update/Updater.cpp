/**
 * @file Updater.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::Updater} class.
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
#include "update/Updater.h"

/**
 * @brief Run a full system update from a TUP file.
 * @param fileSystem where the TUP file is stored and unpacked
 * @param packageFileName full path and name of the package file
 * @return OK when the update was installed
 * @return ERROR_UPDATE_FILE_NOT_FOUND when the update file was not found
 * @return ERROR_INVALID_FILE when the update file is invalid
 * @return ERROR_CLEAN_FS when the FS root could not be cleaned for the update
 * @return ERROR_UPDATE_UNPACK when the update file could not be unpacked
 * @return ERROR_FW_FILE_NOT_FOUND when the firmware file was not found
 * @return ERROR_FW_FILE_EMPTY when the firmware file is empty
 * @return ERROR_OUT_OF_FLASH_MEMORY when there is not enough space in the flash memory
 * @return ERROR_WRITE_FW_DATA when the firmware could not be written
 * @return ERROR_FINISH_FW_UPDATE when the firmware update could not be finalized
 */
NL::Updater::Error NL::Updater::install(FS *fileSystem, const String packageFileName)
{
	if (!NL::FileUtil::fileExists(fileSystem, packageFileName))
	{
		return NL::Updater::Error::ERROR_UPDATE_FILE_NOT_FOUND;
	}

	NL::TupFile tupFile;
	if (tupFile.load(fileSystem, packageFileName) != NL::TupFile::Error::OK)
	{
		fileSystem->remove(packageFileName);
		return NL::Updater::Error::ERROR_INVALID_FILE;
	}

	if (!NL::FileUtil::clearRoot(fileSystem))
	{
		fileSystem->remove(packageFileName);
		return NL::Updater::Error::ERROR_CLEAN_FS;
	}

	if (tupFile.unpack(fileSystem, F("/")) != NL::TupFile::Error::OK)
	{
		fileSystem->remove(packageFileName);
		return NL::Updater::Error::ERROR_UPDATE_UNPACK;
	}

	tupFile.close();
	fileSystem->remove(packageFileName);

	NL::Updater::Error fwError = NL::Updater::installFirmware(fileSystem, F("/firmware.bin"));
	if (fwError != NL::Updater::Error::OK)
	{
		fileSystem->remove(F("/firmware.bin"));
		return fwError;
	}

	fileSystem->remove(F("/firmware.bin"));
	return NL::Updater::Error::OK;
}

/**
 * @brief This function will reboot the controller. It runs in a own task so a delay can be added.
 * @param reason String containing the reason for the reboot
 * @param rebootDelay delay in milliseconds until the controller will reboot, if set to 0 function will not return
 */
void NL::Updater::reboot(const String reason, const uint16_t rebootDelay)
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Rebooting controller in ") + String(rebootDelay / 1000.0f) + F(" seconds for reason: ") + reason);
	if (rebootDelay == 0)
	{
		ESP.restart();
	}
	else
	{
		uint16_t *rDelay = new uint16_t(rebootDelay);
		xTaskCreatePinnedToCore(NL::Updater::rebootInt, ("ResetTask" + String(millis())).c_str(), 1000, (void *)rDelay, 1, NULL, 1);
	}
}

/**
 * @brief Install the firmware file to the flash chip.
 * @param fileSystem where the firmware file is stored
 * @param firmwareFileName full path and name of the firmware file
 * @return OK when the update was successful
 * @return ERROR_FW_FILE_NOT_FOUND when the firmware file was not found
 * @return ERROR_FW_FILE_EMPTY when the firmware file is empty
 * @return ERROR_OUT_OF_FLASH_MEMORY when there is not enough space in the flash memory
 * @return ERROR_WRITE_FW_DATA when the firmware could not be written
 * @return ERROR_FINISH_FW_UPDATE when the firmware update could not be finalized
 */
NL::Updater::Error NL::Updater::installFirmware(FS *fileSystem, const String firmwareFileName)
{
	File firmware = fileSystem->open(firmwareFileName, FILE_READ);
	if (!firmware)
	{
		return NL::Updater::Error::ERROR_FW_FILE_NOT_FOUND;
	}
	else if (firmware.isDirectory())
	{
		firmware.close();
		return NL::Updater::Error::ERROR_FW_FILE_NOT_FOUND;
	}

	const size_t firmwareSize = firmware.size();
	if (firmwareSize == 0)
	{
		firmware.close();
		return NL::Updater::Error::ERROR_FW_FILE_EMPTY;
	}

	if (!Update.begin(firmwareSize))
	{
		firmware.close();
		return NL::Updater::Error::ERROR_OUT_OF_FLASH_MEMORY;
	}

	const size_t writtenBytes = Update.writeStream(firmware);
	if (writtenBytes != firmwareSize)
	{
		firmware.close();
		return NL::Updater::Error::ERROR_WRITE_FW_DATA;
	}

	if (!Update.end())
	{
		firmware.close();
		return NL::Updater::Error::ERROR_FINISH_FW_UPDATE;
	}
	firmware.close();

	if (!Update.isFinished())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Found the following error during firmware update: ") + String(Update.getError()));
		return NL::Updater::Error::ERROR_FINISH_FW_UPDATE;
	}

	return NL::Updater::Error::OK;
}

/**
 * @brief Task function to reboot the controller.
 * @param params parameters for the function call, should contain only the delay
 */
void NL::Updater::rebootInt(void *params)
{
	uint16_t *paramPtr = (uint16_t *)params;
	uint16_t rebootDelay = *paramPtr;
	delete paramPtr;

	delay(rebootDelay);
	ESP.restart();
	vTaskDelete(NULL);
}
