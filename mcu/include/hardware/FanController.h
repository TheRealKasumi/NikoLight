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

		void setPwmMin(const uint16_t pwmMin);
		uint16_t getPwmMin();

		void setPwmMax(const uint16_t pwmMax);
		uint16_t getPwmMax();

		void setTempMin(const uint8_t tempMin);
		uint8_t getTempMin();

		void setTempMax(const uint8_t tempMax);
		uint8_t getTempMax();

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