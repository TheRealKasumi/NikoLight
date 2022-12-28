/**
 * @file MotionSensorEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to calibrate the motion sensor.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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
#ifndef MOTION_SENSOR_ENDPOINT_H
#define MOTION_SENSOR_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"
#include "sensor/MotionSensor.h"

namespace TL
{
	class MotionSensorEndpoint : public RestEndpoint
	{
	public:
		static void begin(TL::Configuration *_configuration, TL::MotionSensor *_motionSensor);

	private:
		MotionSensorEndpoint();

		static TL::Configuration *configuration;
		static TL::MotionSensor *motionSensor;

		static void getCalibrationData();
		static void postCalibrationData();
		static void runCalibration();

		static bool validateConfiguration(const JsonObject &jsonObject);
	};
}

#endif
