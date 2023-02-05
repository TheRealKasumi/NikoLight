/**
 * @file SystemInformation.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::SystemInformation} class.
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
#include "SystemInformation.h"

bool TL::SystemInformation::initialized = false;
TL::SystemInformation::SocInfo TL::SystemInformation::socInfo = TL::SystemInformation::SocInfo();
TL::SystemInformation::HardwareInformation TL::SystemInformation::hardwareInfo = TL::SystemInformation::HardwareInformation();
TL::SystemInformation::TLInformation TL::SystemInformation::systemInfo = TL::SystemInformation::TLInformation();

/**
 * @brief Initialize the {@link TL::SystemInformation}.
 */
void TL::SystemInformation::begin()
{
	TL::SystemInformation::initialized = true;
	strcpy(TL::SystemInformation::socInfo.chipModel, "");
	TL::SystemInformation::socInfo.chipRevision = 0;
	strcpy(TL::SystemInformation::socInfo.fwVersion, FW_VERSION);
	TL::SystemInformation::socInfo.cpuCores = 0;
	TL::SystemInformation::socInfo.cpuClock = 0;
	TL::SystemInformation::socInfo.freeHeap = 0;
	TL::SystemInformation::socInfo.flashSize = 0;
	TL::SystemInformation::socInfo.flashSpeed = 0;
	TL::SystemInformation::socInfo.sketchSize = 0;
	TL::SystemInformation::socInfo.freeSketchSpace = 0;

	strcpy(TL::SystemInformation::hardwareInfo.hwVersion, HW_VERSION);
	TL::SystemInformation::hardwareInfo.regulatorCount = REGULATOR_COUNT;
	TL::SystemInformation::hardwareInfo.regulatorVoltage = REGULATOR_DEFAULT_VOLTAGE;
	TL::SystemInformation::hardwareInfo.regulatorCurrentLimit = REGULATOR_CURRENT_LIMIT * REGULATOR_COUNT;
	TL::SystemInformation::hardwareInfo.regulatorCurrentDraw = 0.0f;
	TL::SystemInformation::hardwareInfo.regulatorPowerLimit = REGULATOR_POWER_LIMIT * REGULATOR_COUNT;
	TL::SystemInformation::hardwareInfo.regulatorPowerDraw = 0;
	TL::SystemInformation::hardwareInfo.regulatorTemperature = 0.0f;
	TL::SystemInformation::hardwareInfo.fanSpeed = 0;
	TL::SystemInformation::hardwareInfo.mpu6050 = 0;
	TL::SystemInformation::hardwareInfo.ds18b20 = 0;
	TL::SystemInformation::hardwareInfo.bh1750 = 0;
	TL::SystemInformation::hardwareInfo.audioUnit = 0;

	TL::SystemInformation::systemInfo.fps = 0;
	TL::SystemInformation::systemInfo.ledCount = 0;

	TL::SystemInformation::updateSocInfo(false);
}

/**
 * @brief Check if the system information was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::SystemInformation::isInitialized()
{
	return TL::SystemInformation::initialized;
}

/**
 * @brief Update the Soc information.
 * @param fast skips slow values that are not changed at runtime
 */
void TL::SystemInformation::updateSocInfo(const bool fast)
{
	if (fast)
	{
		TL::SystemInformation::socInfo.freeHeap = ESP.getFreeHeap();
	}
	else
	{
		strcpy(TL::SystemInformation::socInfo.chipModel, ESP.getChipModel());
		TL::SystemInformation::socInfo.chipRevision = ESP.getChipRevision();
		TL::SystemInformation::socInfo.cpuCores = ESP.getChipCores();
		TL::SystemInformation::socInfo.cpuClock = ESP.getCpuFreqMHz() * 1000000;
		TL::SystemInformation::socInfo.freeHeap = ESP.getFreeHeap();
		TL::SystemInformation::socInfo.flashSize = ESP.getFlashChipSize();
		TL::SystemInformation::socInfo.flashSpeed = ESP.getFlashChipSpeed();
		TL::SystemInformation::socInfo.sketchSize = ESP.getSketchSize();
		TL::SystemInformation::socInfo.freeSketchSpace = ESP.getFreeSketchSpace();
	}
}

/**
 * @brief Get the current Soc information.
 * @return current controller information
 */
TL::SystemInformation::SocInfo TL::SystemInformation::getSocInfo()
{
	return TL::SystemInformation::socInfo;
}

/**
 * @brief Set the current hardware information.
 * @param hardwareInfo current hardware information
 */
void TL::SystemInformation::setHardwareInfo(const TL::SystemInformation::HardwareInformation hardwareInfo)
{
	TL::SystemInformation::hardwareInfo = hardwareInfo;
}

/**
 * @brief Get the current hardware information.
 * @return current hardware information
 */
TL::SystemInformation::HardwareInformation TL::SystemInformation::getHardwareInfo()
{
	return TL::SystemInformation::hardwareInfo;
}

/**
 * @brief Set the current TesLight system information.
 * @param systemInfo new system configuration
 */
void TL::SystemInformation::setTesLightInfo(const TL::SystemInformation::TLInformation systemInfo)
{
	TL::SystemInformation::systemInfo = systemInfo;
}

/**
 * @brief Get the current system information.
 * @return current system information
 */
TL::SystemInformation::TLInformation TL::SystemInformation::getTesLightInfo()
{
	return TL::SystemInformation::systemInfo;
}
