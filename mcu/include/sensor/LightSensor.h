/**
 * @file LightSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the light input, connected to the cars ambient light.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>

#include "logging/Logger.h"

namespace TesLight
{

	class LightSensor
	{
	public:
		enum LightSensorMode
		{
			ALWAYS_OFF = 0,
			ALWAYS_ON = 1,
			AUTO_ON_OFF = 2,
			AUTO_BRIGHTNESS = 3
		};

		LightSensor(const uint8_t inputPin, const uint16_t bufferSize, const TesLight::LightSensor::LightSensorMode lightSensorMode, const uint16_t threshold, const uint16_t minValue, const uint16_t maxValue);
		~LightSensor();

		TesLight::LightSensor::LightSensorMode getLightSensorMode();
		void setLightSensorMode(const TesLight::LightSensor::LightSensorMode lightSensorMode);

		uint16_t getThreshold();
		void setThreshold(const uint16_t threshold);

		uint16_t getMinValue();
		void setMinValue(const uint16_t minValue);

		uint16_t getMaxValue();
		void setMaxValue(const uint16_t maxValue);

		float getBrightness();

	private:
		uint8_t inputPin;
		uint16_t bufferSize;

		TesLight::LightSensor::LightSensorMode lightSensorMode;
		uint16_t threshold;
		uint16_t minValue;
		uint16_t maxValue;

		uint16_t *buffer;
		uint16_t bufferIndex;

		uint16_t getAnalogeValue();
	};
}

#endif