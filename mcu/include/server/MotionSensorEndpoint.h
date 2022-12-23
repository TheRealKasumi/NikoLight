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
