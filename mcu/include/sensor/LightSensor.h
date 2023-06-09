/**
 * @file LightSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the ambient brightness from either the cars ambient light or an external light sensor.
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
#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>

#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "hardware/AnalogInput.h"
#include "hardware/BH1750.h"

#include "sensor/MotionSensor.h"

namespace NL
{
	class LightSensor
	{
	public:
		enum class Error
		{
			OK,						   // No error
			ERROR_CONFIG_UNAVAILABLE,  // The configuration is not available
			ERROR_ADC_UNAVAILABLE,	   // The ADC is not available
			ERROR_BH1750_UNAVAILABLE,  // The BH1750 is not available
			ERROR_MPU6050_UNAVAILABLE, // The MPU6050 is not available
			ERROR_UNKNOWN_MODE,		   // The light sensor mode is unknown
		};

		enum class LightSensorMode : uint8_t
		{
			ALWAYS_OFF = 0,
			ALWAYS_ON = 1,
			AUTO_ON_OFF_ADC = 2,
			AUTO_BRIGHTNESS_ADC = 3,
			AUTO_ON_OFF_BH1750 = 4,
			AUTO_BRIGHTNESS_BH1750 = 5,
			AUTO_ON_OFF_MOTION = 6
		};

		static NL::LightSensor::Error begin();
		static void end();
		static bool isInitialized();

		static NL::LightSensor::Error getBrightness(float &brightness);

	private:
		LightSensor();

		static bool initialized;
		static float lastBrightnessValue;
		static NL::MotionSensor::MotionSensorData motionData;
		static unsigned long motionSensorTriggerTime;

		static NL::LightSensor::Error getBrightnessInt(float &brightness);
	};
}

#endif