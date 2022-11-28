/**
 * @file Updater.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::Updater} class.
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
bool TesLight::Updater::install(FS *fileSystem, const String packageFileName)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Installing system update."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Checking if update package file was found."));
	if (!TesLight::FileUtil::fileExists(fileSystem, packageFileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Update package was not found."));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package was found."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Loading and verifying update package."));
	TesLight::TupFile tupFile;
	if (!tupFile.load(fileSystem, packageFileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Verification of the update package failed. The file must be invalid or is corrupted."));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package verified."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Cleaning file system root."));
	if (!TesLight::FileUtil::clearRoot(fileSystem))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to clean file system root."));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("File system root cleaned. All old data removed."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Unpacking update package."));
	if (!tupFile.unpack(fileSystem, F("/")))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to unpack update package."));
		fileSystem->remove(packageFileName);
		return false;
	}
	tupFile.close();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package was unpacked successfully."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Deleting update package."));
	if (!fileSystem->remove(packageFileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete update package. You should delete it manually."));
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update package was deleted successfully."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Installing firmware file to controller."));
	if (!TesLight::Updater::installFirmware(fileSystem, F("/firmware.bin")))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to install firmware file to controller."));
		fileSystem->remove(F("/firmware.bin"));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware file was installed."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Deleting firmware file."));
	if (!fileSystem->remove(F("/firmware.bin")))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete firmware file. You should delete it manually."));
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware file was deleted successfully."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update completed. Reboot is required."));
	return true;
}

/**
 * @brief This function will reboot the controller. It does not return.
 */
void TesLight::Updater::reboot(const String reason)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Rebooting controller for reason: ") + reason);
	ESP.restart();
}

/**
 * @brief Install the firmware file to the flash chip.
 * @param fileSystem where the firmware file is stored
 * @param firmwareFileName full path and name of the firmware file
 * @return true when the update was successful
 * @return false when there was an error installing the firmware
 */
bool TesLight::Updater::installFirmware(FS *fileSystem, const String firmwareFileName)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Installing firmware file."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Open firmware file."));
	File firmware = fileSystem->open(firmwareFileName, FILE_READ);
	if (!firmware)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open firmware file. The file was not found."));
		return false;
	}
	else if (firmware.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open firmware file. It is a directory. No idea how that could happen."));
		firmware.close();
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware file loaded."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Checking size of the firmware file."));
	const size_t firmwareSize = firmware.size();
	if (firmwareSize == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Firmware file is empty."));
		firmware.close();
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Firmware file has ") + String(firmwareSize) + F("bytes of date."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting firmware update."));
	if (!Update.begin(firmwareSize))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Not enought flash memory to install firmware file."));
		firmware.close();
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware update started."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Writing data to flash memory."));
	const size_t writtenBytes = Update.writeStream(firmware);
	if (writtenBytes != firmwareSize)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Not all data was written. This is not good..."));
		firmware.close();
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("All data was written into the flash memory."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Ending update procedure."));
	if (!Update.end())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to end update procedure: ") + String(Update.getError()));
		firmware.close();
		return false;
	}
	firmware.close();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update procedure ended."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Checking for errors."));
	if (!Update.isFinished())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Found the following error: ") + String(Update.getError()));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No errors found."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Firmware update successful."));
	return true;
}
