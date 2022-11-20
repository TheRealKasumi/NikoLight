/**
 * @file LightSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the ambient brightness from either the cars ambient light or an external light sensor.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>

#include "configuration/SystemConfiguration.h"
#include "hardware/ESP32ADC.h"
#include "hardware/BH1750.h"
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
			AUTO_ON_OFF_ADC = 2,
			AUTO_BRIGHTNESS_ADC = 3,
			AUTO_ON_OFF_BH1750 = 4,
			AUTO_BRIGHTNESS_BH1750 = 5
		};

		LightSensor(const TesLight::LightSensor::LightSensorMode lightSensorMode, const float threshold, const float minValue, const float maxValue);
		~LightSensor();

		TesLight::LightSensor::LightSensorMode getLightSensorMode();
		void setLightSensorMode(const TesLight::LightSensor::LightSensorMode lightSensorMode);

		float getThreshold();
		void setThreshold(const float threshold);

		float getMinValue();
		void setMinValue(const float minValue);

		float getMaxValue();
		void setMaxValue(const float maxValue);

		bool getBrightness(float &brightness);

	private:
		TesLight::ESP32ADC *esp32adc;
		TesLight::BH1750 *bh1750;

		TesLight::LightSensor::LightSensorMode lightSensorMode;
		float threshold;
		float minValue;
		float maxValue;
	};
}

#endif