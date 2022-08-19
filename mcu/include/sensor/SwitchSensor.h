/**
 * @file SwitchSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the switch input.
 * @version 0.0.1
 * @date 2022-08-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SWITCH_SENSOR_H
#define SWITCH_SENSOR_H

#include <stdint.h>

#include "logging/Logger.h"

namespace TesLight
{
	class SwitchSensor
	{
	public:
		SwitchSensor(const uint8_t inputPin, const uint16_t threshold);
		~SwitchSensor();
		bool isActive();

	private:
		uint8_t inputPin;
		uint16_t threshold;
	};
}

#endif