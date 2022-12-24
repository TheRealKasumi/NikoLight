/**
 * @file Updater.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::Updater} class.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "update/Updater.h"

/**
 * @brief Run a full system update from a TUP file.
 * @param fileSystem where the TUP file is stored and unpacked
 * @param packageFileName full path and name of the package file
 * @return true when the update was successful
 * @return false when there was an error
 */
bool TL::Updater::install(FS *fileSystem, const String packageFileName)
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Installing system update."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Checking if update package file was found."));
	if (!TL::FileUtil::fileExists(fileSystem, packageFileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Update package was not found."));
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package was found."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Loading and verifying update package."));
	TL::TupFile tupFile;
	if (!tupFile.load(fileSystem, packageFileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Verification of the update package failed. The file must be invalid or is corrupted."));
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package verified."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Cleaning file system root."));
	if (!TL::FileUtil::clearRoot(fileSystem))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to clean file system root."));
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("File system root cleaned. All old data removed."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Unpacking update package."));
	if (!tupFile.unpack(fileSystem, F("/")))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to unpack update package."));
		fileSystem->remove(packageFileName);
		return false;
	}
	tupFile.close();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package was unpacked successfully."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Deleting update package."));
	if (!fileSystem->remove(packageFileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete update package. You should delete it manually."));
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package was deleted successfully."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Installing firmware file to controller."));
	if (!TL::Updater::installFirmware(fileSystem, F("/firmware.bin")))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to install firmware file to controller."));
		fileSystem->remove(F("/firmware.bin"));
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware file was installed."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Deleting firmware file."));
	if (!fileSystem->remove(F("/firmware.bin")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete firmware file. You should delete it manually."));
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware file was deleted successfully."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update completed. Reboot is required."));
	return true;
}

/**
 * @brief This function will reboot the controller. It runs in a own task so a delay can be added.
 * @param reason String containing the reason for the reboot
 * @param rebootDelay delay in milliseconds until the controller will reboot
 */
void TL::Updater::reboot(const String reason, const uint16_t rebootDelay)
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Rebooting controller in ") + String(rebootDelay / 1000.0f) + F(" seconds for reason: ") + reason);
	uint16_t *rDelay = new uint16_t(rebootDelay);
	xTaskCreatePinnedToCore(TL::Updater::rebootInt, ("ResetTask" + String(millis())).c_str(), 1000, (void *)rDelay, 1, NULL, 1);
}

/**
 * @brief Install the firmware file to the flash chip.
 * @param fileSystem where the firmware file is stored
 * @param firmwareFileName full path and name of the firmware file
 * @return true when the update was successful
 * @return false when there was an error installing the firmware
 */
bool TL::Updater::installFirmware(FS *fileSystem, const String firmwareFileName)
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Installing firmware file."));
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Open firmware file."));
	File firmware = fileSystem->open(firmwareFileName, FILE_READ);
	if (!firmware)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open firmware file. The file was not found."));
		return false;
	}
	else if (firmware.isDirectory())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open firmware file. It is a directory. No idea how that could happen."));
		firmware.close();
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware file loaded."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Checking size of the firmware file."));
	const size_t firmwareSize = firmware.size();
	if (firmwareSize == 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Firmware file is empty."));
		firmware.close();
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Firmware file has ") + String(firmwareSize) + F("bytes of date."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting firmware update."));
	if (!Update.begin(firmwareSize))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Not enought flash memory to install firmware file."));
		firmware.close();
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware update started."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Writing data to flash memory."));
	const size_t writtenBytes = Update.writeStream(firmware);
	if (writtenBytes != firmwareSize)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Not all data was written. This is not good..."));
		firmware.close();
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("All data was written into the flash memory."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Ending update procedure."));
	if (!Update.end())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to end update procedure: ") + String(Update.getError()));
		firmware.close();
		return false;
	}
	firmware.close();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update procedure ended."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Checking for errors."));
	if (!Update.isFinished())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Found the following error: ") + String(Update.getError()));
		return false;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No errors found."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware update successful."));
	return true;
}

/**
 * @brief Task function to reboot the controller.
 * @param params parameters for the function call, should contain only the delay
 */
void TL::Updater::rebootInt(void *params)
{
	uint16_t *paramPtr = (uint16_t *)params;
	uint16_t rebootDelay = *paramPtr;
	delete paramPtr;

	delay(rebootDelay);
	ESP.restart();
	vTaskDelete(NULL);
}
