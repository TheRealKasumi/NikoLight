/**
 * @file LM75BD.h
 * @author TheRealKasumi
 * @brief Contains a class for the LM75BD temperature sensor.
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
#include "configuration/SystemConfiguration.h"
#if defined(HW_VERSION_2_2)

#ifndef LM75BD_H
#define LM75BD_H

#include <stdint.h>
#include <esp32-hal-gpio.h>

#include <FunctionalInterrupt.h>
#include <Wire.h>

namespace NL
{
	class LM75BD
	{
	public:
		// Size of the over temperature fault queue
		enum class OTFaultQueue : uint8_t
		{
			QUEUE_1 = 0b00000000, // Queue size 1
			QUEUE_2 = 0b00001000, // Queue size 2
			QUEUE_4 = 0b00010000, // Queue size 4
			QUEUE_6 = 0b00011000  // Queue size 6 (default)
		};

		// Polarity of the interrupt event
		enum class OTPolarity : uint8_t
		{
			ACTIVE_LOW = 0b00000000, // Interrupt low active (default)
			ACTIVE_HIGH = 0b00000100 // Interrupt high active
		};

		// Operation mode of the over temperature event
		enum class OTOperationMode : uint8_t
		{
			COMP = 0b00000000, // Comparator mode, active when event active (default)
			INTER = 0b00000010 // Interrupt mode, triggers one time
		};

		// Power state of the temperature sensor
		enum class PowerState : uint8_t
		{
			ACTIVE = 0b00000000, // Active (default)
			SLEEP = 0b00000001	 // Shut down
		};

		// Configuration of the temperature sensor
		struct Configuration
		{
			NL::LM75BD::OTFaultQueue overTemperatureFaultQueue;		  // Queue size for fault detection
			NL::LM75BD::OTPolarity overTemperaturePolarity;			  // Polarity of the interrupt
			NL::LM75BD::OTOperationMode overTemperatureOperationMode; // Operation mode of the OT detection
			NL::LM75BD::PowerState powerState;						  // Sower state of the chip
		};

		LM75BD(const uint8_t deviceAddress, const uint8_t interruptPin = 0);
		~LM75BD();

		void setConfiguration(const NL::LM75BD::Configuration configuration);
		const NL::LM75BD::Configuration getConfiguration();

		void setOverTemperatureThreshold(float threshold);
		const float getOverTemperatureThreshold();

		void setOverTemperatureHysteresis(float hysteresis);
		const float getOverTemperatureHysteresis();

		static void setAlarmCallback(void (*alarmCallback)());

		const float getTemperature();

	private:
		uint8_t deviceAddress;
		uint8_t interruptPin;

		static void IRAM_ATTR (*alarmCallback)();
		static void IRAM_ATTR alarmInterruptHandler();
	};
}

#endif
#endif
