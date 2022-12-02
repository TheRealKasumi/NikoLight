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
#include "configuration/Configuration.h"
#include "hardware/ESP32ADC.h"
#include "hardware/BH1750.h"
#include "logging/Logger.h"

#include "sensor/MotionSensor.h"

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
			AUTO_BRIGHTNESS_BH1750 = 5,
			AUTO_ON_OFF_MPU6050 = 6
		};

		LightSensor(TesLight::Configuration *configuration);
		~LightSensor();

		bool getBrightness(float &brightness, TesLight::MotionSensor *motionSensor = nullptr);

	private:
		TesLight::Configuration *configuration;
		TesLight::ESP32ADC *esp32adc;
		TesLight::BH1750 *bh1750;
		TesLight::MotionSensor::MotionSensorData motionData;
		unsigned long motionSensorTriggerTime;
	};
}

#endif