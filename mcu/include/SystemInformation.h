/**
 * @file SystemInformation.h
 * @author TheRealKasumi
 * @brief Contains a class to hold information about the system like available resources.
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
#ifndef SYSTEM_INFORMATION_H
#define SYSTEM_INFORMATION_H

#include <stdint.h>
#include <WString.h>
#include <Esp.h>
#include "configuration/SystemConfiguration.h"

namespace NL
{
	class SystemInformation
	{
	public:
		struct SocInfo
		{
			String chipModel;
			uint8_t chipRevision;
			String fwVersion;
			uint8_t cpuCores;
			uint32_t cpuClock;
			uint32_t freeHeap;
			uint32_t flashSize;
			uint32_t flashSpeed;
			uint32_t sketchSize;
			uint32_t freeSketchSpace;
		};

		struct HardwareInformation
		{
			String hwVersion;
			uint8_t regulatorCount;
			float regulatorVoltage;
			float regulatorCurrentLimit;
			float regulatorCurrentDraw;
			float regulatorPowerLimit;
			float regulatorPowerDraw;
			float regulatorTemperature;
			uint8_t fanSpeed;
			uint8_t mpu6050;
			uint8_t ds18b20;
			uint8_t bh1750;
			uint8_t audioUnit;
		};

		struct NLInformation
		{
			float fps;
			uint16_t ledCount;
			uint16_t hiddenLedCount;
		};

		static void begin();
		static bool isInitialized();

		static void updateSocInfo(const bool fast = true);
		static NL::SystemInformation::SocInfo getSocInfo();

		static void setHardwareInfo(const NL::SystemInformation::HardwareInformation hardwareInfo);
		static NL::SystemInformation::HardwareInformation getHardwareInfo();

		static void setNikoLightInfo(const NL::SystemInformation::NLInformation systemInfo);
		static NL::SystemInformation::NLInformation getNikoLightInfo();

	private:
		SystemInformation();

		static bool initialized;
		static NL::SystemInformation::SocInfo socInfo;
		static NL::SystemInformation::HardwareInformation hardwareInfo;
		static NL::SystemInformation::NLInformation systemInfo;
	};
}

#endif