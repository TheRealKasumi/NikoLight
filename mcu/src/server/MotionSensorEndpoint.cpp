/**
 * @file MotionSensorEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to calibrate the motion sensor.
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
#include "server/MotionSensorEndpoint.h"

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::MotionSensorEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/motion/calibration")).c_str(), http_method::HTTP_GET, NL::MotionSensorEndpoint::getCalibrationData);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/motion/calibration")).c_str(), http_method::HTTP_PATCH, NL::MotionSensorEndpoint::patchCalibrationData);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/motion/calibration/auto")).c_str(), http_method::HTTP_PATCH, NL::MotionSensorEndpoint::runCalibration);
}

/**
 * @brief Return the calibration data to the client.
 */
void NL::MotionSensorEndpoint::getCalibrationData()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the motion sensor calibration data."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(512);
	const JsonObject calibration = jsonDoc.createNestedObject(F("motionSensorCalibration"));
	calibration[F("accXRaw")] = NL::Configuration::getMotionSensorCalibration().accXRaw;
	calibration[F("accYRaw")] = NL::Configuration::getMotionSensorCalibration().accYRaw;
	calibration[F("accZRaw")] = NL::Configuration::getMotionSensorCalibration().accZRaw;
	calibration[F("gyroXRaw")] = NL::Configuration::getMotionSensorCalibration().gyroXRaw;
	calibration[F("gyroYRaw")] = NL::Configuration::getMotionSensorCalibration().gyroYRaw;
	calibration[F("gyroZRaw")] = NL::Configuration::getMotionSensorCalibration().gyroZRaw;
	calibration[F("accXG")] = NL::Configuration::getMotionSensorCalibration().accXG;
	calibration[F("accYG")] = NL::Configuration::getMotionSensorCalibration().accYG;
	calibration[F("accZG")] = NL::Configuration::getMotionSensorCalibration().accZG;
	calibration[F("gyroXDeg")] = NL::Configuration::getMotionSensorCalibration().gyroXDeg;
	calibration[F("gyroYDeg")] = NL::Configuration::getMotionSensorCalibration().gyroYDeg;
	calibration[F("gyroZDeg")] = NL::Configuration::getMotionSensorCalibration().gyroZDeg;

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::MotionSensorEndpoint::sendJsonDocument(200, F("Here is the motion sensor calibration."), jsonDoc);
}

/**
 * @brief Update the motion sensor calibration manually.
 */
void NL::MotionSensorEndpoint::patchCalibrationData()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the motion sensor calibration data."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::MotionSensorEndpoint::webServer->hasHeader(F("content-type")) || NL::MotionSensorEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::MotionSensorEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the motion sensor calibration."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the motion sensor calibration."));
		return;
	}

	const String body = NL::MotionSensorEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(512);
	if (!NL::MotionSensorEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("motionSensorCalibration")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"motionSensorCalibration\" object."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The json must contain a \"motionSensorCalibration\" object."));
		return;
	}

	const JsonObject calibration = jsonDoc[F("motionSensorCalibration")].as<JsonObject>();
	if (!NL::MotionSensorEndpoint::validateConfiguration(calibration))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the motion sensor calibration failed."));
		return;
	}

	NL::Configuration::MotionSensorCalibration motionSensorCalibration;
	motionSensorCalibration.accXRaw = calibration[F("accXRaw")].as<int16_t>();
	motionSensorCalibration.accYRaw = calibration[F("accYRaw")].as<int16_t>();
	motionSensorCalibration.accZRaw = calibration[F("accZRaw")].as<int16_t>();
	motionSensorCalibration.gyroXRaw = calibration[F("gyroXRaw")].as<int16_t>();
	motionSensorCalibration.gyroYRaw = calibration[F("gyroYRaw")].as<int16_t>();
	motionSensorCalibration.gyroZRaw = calibration[F("gyroZRaw")].as<int16_t>();
	motionSensorCalibration.accXG = calibration[F("accXG")].as<float>();
	motionSensorCalibration.accYG = calibration[F("accYG")].as<float>();
	motionSensorCalibration.accZG = calibration[F("accZG")].as<float>();
	motionSensorCalibration.gyroXDeg = calibration[F("gyroXDeg")].as<float>();
	motionSensorCalibration.gyroYDeg = calibration[F("gyroYDeg")].as<float>();
	motionSensorCalibration.gyroZDeg = calibration[F("gyroZDeg")].as<float>();

	NL::Configuration::setMotionSensorCalibration(motionSensorCalibration);
	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration. The configuration file could not be opened."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration. The configuration file could not be written."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Calibration data saved. Sending the response."));
	NL::MotionSensorEndpoint::sendSimpleResponse(200, F("I saved the motion data calibration for you. I want a cookie..."));
}

/**
 * @brief Automatically calibrate the motion sensor.
 */
void NL::MotionSensorEndpoint::runCalibration()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to automatically calibrate the motion sensor."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::MotionSensor::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The motion sensor is not available. Can not calibrate. Make sure to connect the MPU6050 correctly to the I²C bus and restart."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The motion sensor is not available. Can not calibrate. Make sure to connect the MPU6050 correctly to the I²C bus and restart."));
		return;
	}

	if (!NL::MotionSensor::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor because it was not initialized."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("Can not calibrate motion sensor because it was not initialized."));
		return;
	}

	const NL::MotionSensor::Error calibrationError = NL::MotionSensor::calibrate(true);
	if (calibrationError == NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to calibrate the motion sensor. The MPU6050 is not available."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to calibrate the motion sensor. The MPU6050 is not available."));
		return;
	}
	else if (calibrationError == NL::MotionSensor::Error::ERROR_TOO_COLD)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too low."));
		NL::MotionSensorEndpoint::sendSimpleResponse(412, F("Can not calibrate motion sensor becaues the temperature is too low."));
		return;
	}
	else if (calibrationError == NL::MotionSensor::Error::ERROR_TOO_WARM)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too high."));
		NL::MotionSensorEndpoint::sendSimpleResponse(412, F("Can not calibrate motion sensor becaues the temperature is too high."));
		return;
	}

	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration data."));
		NL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration data."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Calibration data saved. Sending the response."));
	NL::MotionSensorEndpoint::sendSimpleResponse(200, F("I calibrated the motion sensor for you."));
}

/**
 * @brief Validate if the motion sensor calibration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool NL::MotionSensorEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("accXRaw")].is<int16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accXRaw\" field must be of type \"int16\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accXRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("accYRaw")].is<int16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accYRaw\" field must be of type \"int16\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accYRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("accZRaw")].is<int16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accZRaw\" field must be of type \"int16\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accZRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("gyroXRaw")].is<int16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroXRaw\" field must be of type \"int16\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroXRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("gyroYRaw")].is<int16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroYRaw\" field must be of type \"int16\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroYRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("gyroZRaw")].is<int16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroZRaw\" field must be of type \"int16\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroZRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("accXG")].is<float>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accXG\" field must be of type \"float\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accXG\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("accYG")].is<float>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accYG\" field must be of type \"float\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accYG\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("accZG")].is<float>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accZG\" field must be of type \"float\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accZG\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("gyroXDeg")].is<float>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroXDeg\" field must be of type \"float\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroXDeg\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("gyroYDeg")].is<float>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroYDeg\" field must be of type \"float\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroYDeg\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("gyroZDeg")].is<float>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroZDeg\" field must be of type \"float\"."));
		NL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroZDeg\" field must be of type \"float\"."));
		return false;
	}

	return true;
}