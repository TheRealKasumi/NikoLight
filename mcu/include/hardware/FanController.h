/**
 * @file FanController.h
 * @author TheRealKasumi
 * @brief Contains a class to control the cooling fan.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>
#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "logging/Logger.h"

namespace TL
{
	class FanController
	{
	public:
		FanController(const uint8_t fanPin, TL::Configuration *configuration);
		~FanController();

		void setTemperature(const uint8_t temp);

	private:
		uint8_t fanPin;
		TL::Configuration *configuration;
	};
}

#endif