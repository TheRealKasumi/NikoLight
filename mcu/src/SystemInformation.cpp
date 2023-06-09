/**
 * @file SystemInformation.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::SystemInformation} class.
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

bool NL::SystemInformation::initialized = false;
NL::SystemInformation::SocInfo NL::SystemInformation::socInfo = NL::SystemInformation::SocInfo();
NL::SystemInformation::HardwareInformation NL::SystemInformation::hardwareInfo = NL::SystemInformation::HardwareInformation();
NL::SystemInformation::NLInformation NL::SystemInformation::systemInfo = NL::SystemInformation::NLInformation();

/**
 * @brief Initialize the {@link NL::SystemInformation}.
 */
void NL::SystemInformation::begin()
{
	NL::SystemInformation::initialized = true;
	NL::SystemInformation::socInfo.chipModel = String();
	NL::SystemInformation::socInfo.chipRevision = 0;
	NL::SystemInformation::socInfo.fwVersion = FW_VERSION;
	NL::SystemInformation::socInfo.cpuCores = 0;
	NL::SystemInformation::socInfo.cpuClock = 0;
	NL::SystemInformation::socInfo.freeHeap = 0;
	NL::SystemInformation::socInfo.flashSize = 0;
	NL::SystemInformation::socInfo.flashSpeed = 0;
	NL::SystemInformation::socInfo.sketchSize = 0;
	NL::SystemInformation::socInfo.freeSketchSpace = 0;

	NL::SystemInformation::hardwareInfo.hwVersion = HW_VERSION;
	NL::SystemInformation::hardwareInfo.regulatorCount = REGULATOR_COUNT;
	NL::SystemInformation::hardwareInfo.regulatorVoltage = REGULATOR_VOLTAGE;
	NL::SystemInformation::hardwareInfo.regulatorCurrentLimit = REGULATOR_CURRENT_LIMIT * REGULATOR_COUNT;
	NL::SystemInformation::hardwareInfo.regulatorCurrentDraw = 0.0f;
	NL::SystemInformation::hardwareInfo.regulatorPowerLimit = REGULATOR_POWER_LIMIT * REGULATOR_COUNT;
	NL::SystemInformation::hardwareInfo.regulatorPowerDraw = 0;
	NL::SystemInformation::hardwareInfo.regulatorTemperature = 0.0f;
	NL::SystemInformation::hardwareInfo.fanSpeed = 0;
	NL::SystemInformation::hardwareInfo.mpu6050 = 0;
	NL::SystemInformation::hardwareInfo.ds18b20 = 0;
	NL::SystemInformation::hardwareInfo.bh1750 = 0;
	NL::SystemInformation::hardwareInfo.audioUnit = 0;

	NL::SystemInformation::systemInfo.fps = 0;
	NL::SystemInformation::systemInfo.ledCount = 0;

	NL::SystemInformation::updateSocInfo(false);
}

/**
 * @brief Check if the system information was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::SystemInformation::isInitialized()
{
	return NL::SystemInformation::initialized;
}

/**
 * @brief Update the Soc information.
 * @param fast skips slow values that are not changed at runtime
 */
void NL::SystemInformation::updateSocInfo(const bool fast)
{
	if (fast)
	{
		NL::SystemInformation::socInfo.freeHeap = ESP.getFreeHeap();
	}
	else
	{
		NL::SystemInformation::socInfo.chipModel = String(ESP.getChipModel());
		NL::SystemInformation::socInfo.chipRevision = ESP.getChipRevision();
		NL::SystemInformation::socInfo.cpuCores = ESP.getChipCores();
		NL::SystemInformation::socInfo.cpuClock = ESP.getCpuFreqMHz() * 1000000;
		NL::SystemInformation::socInfo.freeHeap = ESP.getFreeHeap();
		NL::SystemInformation::socInfo.flashSize = ESP.getFlashChipSize();
		NL::SystemInformation::socInfo.flashSpeed = ESP.getFlashChipSpeed();
		NL::SystemInformation::socInfo.sketchSize = ESP.getSketchSize();
		NL::SystemInformation::socInfo.freeSketchSpace = ESP.getFreeSketchSpace();
	}
}

/**
 * @brief Get the current Soc information.
 * @return current controller information
 */
NL::SystemInformation::SocInfo NL::SystemInformation::getSocInfo()
{
	return NL::SystemInformation::socInfo;
}

/**
 * @brief Set the current hardware information.
 * @param hardwareInfo current hardware information
 */
void NL::SystemInformation::setHardwareInfo(const NL::SystemInformation::HardwareInformation hardwareInfo)
{
	NL::SystemInformation::hardwareInfo = hardwareInfo;
}

/**
 * @brief Get the current hardware information.
 * @return current hardware information
 */
NL::SystemInformation::HardwareInformation NL::SystemInformation::getHardwareInfo()
{
	return NL::SystemInformation::hardwareInfo;
}

/**
 * @brief Set the current NikoLight system information.
 * @param systemInfo new system configuration
 */
void NL::SystemInformation::setNikoLightInfo(const NL::SystemInformation::NLInformation systemInfo)
{
	NL::SystemInformation::systemInfo = systemInfo;
}

/**
 * @brief Get the current system information.
 * @return current system information
 */
NL::SystemInformation::NLInformation NL::SystemInformation::getNikoLightInfo()
{
	return NL::SystemInformation::systemInfo;
}
