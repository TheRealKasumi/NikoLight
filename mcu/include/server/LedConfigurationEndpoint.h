/**
 * @file LedConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to configure the LED settings.
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
#ifndef LED_CONFIGURATION_ENDPOINT_H
#define LED_CONFIGURATION_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "led/LedManager.h"
#include "sensor/MotionSensor.h"
#include "logging/Logger.h"

namespace TL
{
	class LedConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin();

	private:
		LedConfigurationEndpoint();

		static void getLedConfig();
		static void postLedConfig();

		static bool validateLedZone(const JsonObject &jsonObject, const uint8_t index);
		static bool validateAnimationSettings(const uint8_t type, const JsonArray &jsonArray);
		static bool isValidPin(const uint8_t pinNumber);
		static bool isInRange(const long value, const long min, const long max);
		static bool isInRange(const float value, const float min, const float max);
	};
}

#endif
