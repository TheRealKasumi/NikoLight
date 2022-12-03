/**
 * @file MotionSensorEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to calibrate the motion sensor.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef MOTION_SENSOR_ENDPOINT_H
#define MOTION_SENSOR_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "util/InMemoryBinaryFile.h"
#include "logging/Logger.h"
#include "sensor/MotionSensor.h"

namespace TesLight
{
	class MotionSensorEndpoint : public RestEndpoint
	{
	public:
		static void begin(TesLight::Configuration *_configuration, TesLight::MotionSensor *_motionSensor);

	private:
		MotionSensorEndpoint();

		static TesLight::Configuration *configuration;
		static TesLight::MotionSensor *motionSensor;

		static void getCalibrationData();
		static void postCalibrationData();
		static void runCalibration();
	};
}

#endif
