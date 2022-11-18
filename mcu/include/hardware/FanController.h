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
#include "logging/Logger.h"

namespace TesLight
{

	class FanController
	{
	public:
		FanController(const uint8_t fanPin, const uint16_t pwmMin, const uint16_t pwmMax, const uint8_t tempMin, const uint8_t tempMax);
		~FanController();

		void setTemperature(const uint8_t temp);

	private:
		uint8_t fanPin;
		uint16_t pwmMin;
		uint16_t pwmMax;
		uint8_t tempMin;
		uint8_t tempMax;
	};
}

#endif