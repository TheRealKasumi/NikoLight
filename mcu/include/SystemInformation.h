/**
 * @file SystemInformation.h
 * @author TheRealKasumi
 * @brief Contains a class to hold information about the system like available resources.
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