/**
 * @file MotionSensorEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to calibrate the motion sensor.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef MOTION_SENSOR_ENDPOINT_H
#define MOTION_SENSOR_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"
#include "sensor/MotionSensor.h"

namespace NL
{
	class MotionSensorEndpoint : public RestEndpoint
	{
	public:
		static void begin();

	private:
		MotionSensorEndpoint();

		static void getCalibrationData();
		static void patchCalibrationData();
		static void runCalibration();

		static bool validateConfiguration(const JsonObject &jsonObject);
	};
}

#endif
