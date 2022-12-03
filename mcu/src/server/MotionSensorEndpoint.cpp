/**
 * @file MotionSensorEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to calibrate the motion sensor.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/MotionSensorEndpoint.h"

// Initialize
TesLight::Configuration *TesLight::MotionSensorEndpoint::configuration = nullptr;
TesLight::MotionSensor *TesLight::MotionSensorEndpoint::motionSensor = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::MotionSensorEndpoint::begin(TesLight::Configuration *_configuration, TesLight::MotionSensor *_motionSensor)
{
	TesLight::MotionSensorEndpoint::configuration = _configuration;
	TesLight::MotionSensorEndpoint::motionSensor = _motionSensor;

	webServerManager->addRequestHandler((getBaseUri() + F("config/motion")).c_str(), http_method::HTTP_GET, TesLight::MotionSensorEndpoint::getCalibrationData);
	webServerManager->addRequestHandler((getBaseUri() + F("config/motion")).c_str(), http_method::HTTP_POST, TesLight::MotionSensorEndpoint::postCalibrationData);
	webServerManager->addRequestHandler((getBaseUri() + F("config/motion")).c_str(), http_method::HTTP_PATCH, TesLight::MotionSensorEndpoint::runCalibration);
}

/**
 * @brief Return the calibration data to the client as binary data.
 */
void TesLight::MotionSensorEndpoint::getCalibrationData()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the motion sensor calibration data."));

	TesLight::InMemoryBinaryFile binary(36);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().accXRaw);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().accYRaw);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().accZRaw);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().gyroXRaw);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().gyroYRaw);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().gyroZRaw);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().accXG);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().accYG);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().accZG);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().gyroXDeg);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().gyroYDeg);
	binary.write(TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration().gyroZDeg);

	const String encoded = TesLight::Base64Util::encode(binary.getData(), binary.getBytesWritten());
	if (encoded == F("BASE64_ERROR"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to encode response."));
		webServer->send(500, F("application/octet-stream"), F("Failed to encode response."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, F("application/octet-stream"), encoded);
}

/**
 * @brief Receive the motion sensor calibration sent by the client.
 */
void TesLight::MotionSensorEndpoint::postCalibrationData()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the motion sensor calibration data."));

	if (!webServer->hasArg(F("data")) || webServer->arg(F("data")).length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a x-www-form-urlencoded body parameter \"data\" with the base64 encoded calibration data."));
		webServer->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded calibration data."));
		return;
	}

	const String encoded = webServer->arg(F("data"));
	size_t length;
	uint8_t *decoded = TesLight::Base64Util::decode(encoded, length);
	if (decoded == nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to decode request."));
		webServer->send(500, F("application/octet-stream"), F("Failed to decode request."));
		return;
	}

	if (length != 36)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Length of decoded data is invalid."));
		delete[] decoded;
		webServer->send(400, F("text/plain"), F("The length of the decoded data must be exactly 36 bytes."));
		return;
	}

	TesLight::InMemoryBinaryFile binary(length);
	binary.loadFrom(decoded, length);
	delete[] decoded;

	TesLight::Configuration::MotionSensorCalibration motionSensorCalibration = TesLight::MotionSensorEndpoint::configuration->getMotionSensorCalibration();
	binary.read(motionSensorCalibration.accXRaw);
	binary.read(motionSensorCalibration.accYRaw);
	binary.read(motionSensorCalibration.accZRaw);
	binary.read(motionSensorCalibration.gyroXRaw);
	binary.read(motionSensorCalibration.gyroYRaw);
	binary.read(motionSensorCalibration.gyroZRaw);
	binary.read(motionSensorCalibration.accXG);
	binary.read(motionSensorCalibration.accYG);
	binary.read(motionSensorCalibration.accZG);
	binary.read(motionSensorCalibration.gyroXDeg);
	binary.read(motionSensorCalibration.gyroYDeg);
	binary.read(motionSensorCalibration.gyroZDeg);
	TesLight::MotionSensorEndpoint::configuration->setMotionSensorCalibration(motionSensorCalibration);

	if (!TesLight::MotionSensorEndpoint::configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration data."));
		webServer->send(500, F("text/plain"), F("Failed to save motion sensor calibration data."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Calibration data saved. Sending the response."));
	webServer->send(200);
}

/**
 * @brief Automatically calibrate the motion sensor.
 */
void TesLight::MotionSensorEndpoint::runCalibration()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to automatically calibrate the motion sensor."));

	uint8_t result = TesLight::MotionSensorEndpoint::motionSensor->calibrate(true);
	if (result == 1)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read motion sensor."));
		webServer->send(500, F("text/plain"), F("Failed to read motion sensor."));
		return;
	}
	else if (result == 2)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too low."));
		webServer->send(500, F("text/plain"), F("Can not calibrate motion sensor becaues the temperature is too low."));
		return;
	}
	else if (result == 3)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too high."));
		webServer->send(500, F("text/plain"), F("Can not calibrate motion sensor becaues the temperature is too high."));
		return;
	}

	if (!TesLight::MotionSensorEndpoint::configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save motion sensor calibration data."));
		webServer->send(500, F("text/plain"), F("Failed to save motion sensor calibration data."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Calibration data saved. Sending the response."));
	webServer->send(200);
}