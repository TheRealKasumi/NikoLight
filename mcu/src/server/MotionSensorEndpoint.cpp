/**
 * @file MotionSensorEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to calibrate the motion sensor.
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
#include "server/MotionSensorEndpoint.h"

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::MotionSensorEndpoint::begin()
{
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/motion")).c_str(), http_method::HTTP_GET, TL::MotionSensorEndpoint::getCalibrationData);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/motion")).c_str(), http_method::HTTP_POST, TL::MotionSensorEndpoint::postCalibrationData);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/motion")).c_str(), http_method::HTTP_PATCH, TL::MotionSensorEndpoint::runCalibration);
}

/**
 * @brief Return the calibration data to the client.
 */
void TL::MotionSensorEndpoint::getCalibrationData()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the motion sensor calibration data."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(512);
	const JsonObject calibration = jsonDoc.createNestedObject(F("motionSensorCalibration"));
	calibration[F("accXRaw")] = TL::Configuration::getMotionSensorCalibration().accXRaw;
	calibration[F("accYRaw")] = TL::Configuration::getMotionSensorCalibration().accYRaw;
	calibration[F("accZRaw")] = TL::Configuration::getMotionSensorCalibration().accZRaw;
	calibration[F("gyroXRaw")] = TL::Configuration::getMotionSensorCalibration().gyroXRaw;
	calibration[F("gyroYRaw")] = TL::Configuration::getMotionSensorCalibration().gyroYRaw;
	calibration[F("gyroZRaw")] = TL::Configuration::getMotionSensorCalibration().gyroZRaw;
	calibration[F("accXG")] = TL::Configuration::getMotionSensorCalibration().accXG;
	calibration[F("accYG")] = TL::Configuration::getMotionSensorCalibration().accYG;
	calibration[F("accZG")] = TL::Configuration::getMotionSensorCalibration().accZG;
	calibration[F("gyroXDeg")] = TL::Configuration::getMotionSensorCalibration().gyroXDeg;
	calibration[F("gyroYDeg")] = TL::Configuration::getMotionSensorCalibration().gyroYDeg;
	calibration[F("gyroZDeg")] = TL::Configuration::getMotionSensorCalibration().gyroZDeg;

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::MotionSensorEndpoint::sendJsonDocument(200, F("Here is the motion sensor calibration."), jsonDoc);
}

/**
 * @brief Receive the motion sensor calibration sent by the client.
 */
void TL::MotionSensorEndpoint::postCalibrationData()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the motion sensor calibration data."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::MotionSensorEndpoint::webServer->hasHeader(F("content-type")) || TL::MotionSensorEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::MotionSensorEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the motion sensor calibration."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the motion sensor calibration."));
		return;
	}

	const String body = TL::MotionSensorEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(512);
	if (!TL::MotionSensorEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("motionSensorCalibration")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain the \"motionSensorCalibration\" object."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The json must contain the \"motionSensorCalibration\" object."));
		return;
	}

	const JsonObject calibration = jsonDoc[F("motionSensorCalibration")].as<JsonObject>();
	if (!TL::MotionSensorEndpoint::validateConfiguration(calibration))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the motion sensor calibration failed."));
		return;
	}

	TL::Configuration::MotionSensorCalibration motionSensorCalibration;
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

	TL::Configuration::setMotionSensorCalibration(motionSensorCalibration);
	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration. The configuration file could not be opened."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration. The configuration file could not be written."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Calibration data saved. Sending the response."));
	TL::MotionSensorEndpoint::sendSimpleResponse(200, F("I saved the motion data calibration for you. I want a cookie..."));
}

/**
 * @brief Automatically calibrate the motion sensor.
 */
void TL::MotionSensorEndpoint::runCalibration()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to automatically calibrate the motion sensor."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::MotionSensor::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The motion sensor is not available. Can not calibrate. Make sure to connect the MPU6050 correctly to the I²C bus and restart."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The motion sensor is not available. Can not calibrate. Make sure to connect the MPU6050 correctly to the I²C bus and restart."));
		return;
	}

	if (!TL::MotionSensor::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor because it was not initialized."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("Can not calibrate motion sensor because it was not initialized."));
		return;
	}

	const TL::MotionSensor::Error calibrationError = TL::MotionSensor::calibrate(true);
	if (calibrationError == TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to calibrate the motion sensor. The MPU6050 is not available."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to calibrate the motion sensor. The MPU6050 is not available."));
		return;
	}
	else if (calibrationError == TL::MotionSensor::Error::ERROR_TOO_COLD)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too low."));
		TL::MotionSensorEndpoint::sendSimpleResponse(412, F("Can not calibrate motion sensor becaues the temperature is too low."));
		return;
	}
	else if (calibrationError == TL::MotionSensor::Error::ERROR_TOO_WARM)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too high."));
		TL::MotionSensorEndpoint::sendSimpleResponse(412, F("Can not calibrate motion sensor becaues the temperature is too high."));
		return;
	}

	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration data."));
		TL::MotionSensorEndpoint::sendSimpleResponse(500, F("Failed to save motion sensor calibration data."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Calibration data saved. Sending the response."));
	TL::MotionSensorEndpoint::sendSimpleResponse(200, F("I calibrated the motion sensor for you."));
}

/**
 * @brief Validate if the motion sensor calibration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool TL::MotionSensorEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("accXRaw")].is<int16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accXRaw\" field must be of type \"int16\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accXRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("accYRaw")].is<int16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accYRaw\" field must be of type \"int16\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accYRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("accZRaw")].is<int16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accZRaw\" field must be of type \"int16\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accZRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("gyroXRaw")].is<int16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroXRaw\" field must be of type \"int16\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroXRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("gyroYRaw")].is<int16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroYRaw\" field must be of type \"int16\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroYRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("gyroZRaw")].is<int16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroZRaw\" field must be of type \"int16\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroZRaw\" field must be of type \"int16\"."));
		return false;
	}

	if (!jsonObject[F("accXG")].is<float>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accXG\" field must be of type \"float\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accXG\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("accYG")].is<float>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accYG\" field must be of type \"float\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accYG\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("accZG")].is<float>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accZG\" field must be of type \"float\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"accZG\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("gyroXDeg")].is<float>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroXDeg\" field must be of type \"float\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroXDeg\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("gyroYDeg")].is<float>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroYDeg\" field must be of type \"float\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroYDeg\" field must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("gyroZDeg")].is<float>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"gyroZDeg\" field must be of type \"float\"."));
		TL::MotionSensorEndpoint::sendSimpleResponse(400, F("The \"gyroZDeg\" field must be of type \"float\"."));
		return false;
	}

	return true;
}