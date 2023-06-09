/**
 * @file LedConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to configure the LED settings.
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
#ifndef LED_CONFIGURATION_ENDPOINT_H
#define LED_CONFIGURATION_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "led/LedManager.h"
#include "sensor/MotionSensor.h"
#include "hardware/AudioUnit.h"
#include "logging/Logger.h"

namespace NL
{
	class LedConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin();

	private:
		LedConfigurationEndpoint();

		static void getLedConfig();
		static void patchLedConfig();

		static bool validateLedZone(const JsonObject &jsonObject, const uint8_t index);
		static bool validateAnimationSettings(const uint8_t type, const JsonArray &jsonArray);
		static bool isValidPin(const uint8_t pinNumber);
		static bool isInRange(const long value, const long min, const long max);
		static bool isInRange(const float value, const float min, const float max);
	};
}

#endif
