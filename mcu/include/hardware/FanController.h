/**
 * @file FanController.h
 * @author TheRealKasumi
 * @brief Contains a class to control the cooling fan.
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
#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>
#include "configuration/Configuration.h"

namespace TL
{
	class FanController
	{
	public:
		enum class Error
		{
			OK,						  // No error
			ERROR_CONFIG_UNAVAILABLE, // The configuration is not available
			ERROR_SETUP_PIN			  // Error setting up the pin
		};

		static TL::FanController::Error begin(const uint8_t fanPin, const uint8_t pwmChannel, const uint32_t frequency, const uint8_t resolution);
		static void end();
		static bool isInitialized();

		static void setTemperature(const uint8_t temp);

	private:
		FanController();

		static bool initialized;
		static uint8_t fanPin;
		static uint8_t pwmChannel;
		static uint32_t frequency;
		static uint8_t resolution;
	};
}

#endif