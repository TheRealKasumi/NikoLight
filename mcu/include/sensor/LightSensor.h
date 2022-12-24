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
#include <memory>

#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "hardware/ESP32ADC.h"
#include "hardware/BH1750.h"
#include "logging/Logger.h"

#include "sensor/MotionSensor.h"

namespace TL
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

		LightSensor(TL::Configuration *configuration);
		~LightSensor();

		bool getBrightness(float &brightness, TL::MotionSensor *motionSensor = nullptr);

	private:
		TL::Configuration *configuration;
		std::unique_ptr<TL::ESP32ADC> esp32adc;
		std::unique_ptr<TL::BH1750> bh1750;
		float lastBrightnessValue;
		TL::MotionSensor::MotionSensorData motionData;
		unsigned long motionSensorTriggerTime;

		bool getBrightnessInt(float &brightness, TL::MotionSensor *motionSensor = nullptr);
	};
}

#endif