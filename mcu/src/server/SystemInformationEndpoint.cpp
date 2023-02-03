/**
 * @file SystemInformationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to get system information.
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
#include "server/SystemInformationEndpoint.h"

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::SystemInformationEndpoint::begin()
{
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("info/system")).c_str(), http_method::HTTP_GET, TL::SystemInformationEndpoint::getSystemInformation);
}

/**
 * @brief Return the system information/status to the client.
 */
void TL::SystemInformationEndpoint::getSystemInformation()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the system information/status."));
	if (!TL::SystemInformation::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight system information was not initialized. Can not access."));
		TL::SystemInformationEndpoint::sendSimpleResponse(500, F("The TesLight system information was not initialized. Can not access."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);

	const JsonObject socInfo = jsonDoc.createNestedObject(F("socInfo"));
	socInfo[F("chipModel")] = TL::SystemInformation::getSocInfo().chipModel;
	socInfo[F("chipRevision")] = TL::SystemInformation::getSocInfo().chipRevision;
	socInfo[F("fwVersion")] = TL::SystemInformation::getSocInfo().fwVersion;
	socInfo[F("cpuCores")] = TL::SystemInformation::getSocInfo().cpuCores;
	socInfo[F("cpuClock")] = TL::SystemInformation::getSocInfo().cpuClock;
	socInfo[F("freeHeap")] = TL::SystemInformation::getSocInfo().freeHeap;
	socInfo[F("flashSize")] = TL::SystemInformation::getSocInfo().flashSize;
	socInfo[F("flashSpeed")] = TL::SystemInformation::getSocInfo().flashSpeed;
	socInfo[F("sketchSize")] = TL::SystemInformation::getSocInfo().sketchSize;
	socInfo[F("freeSketchSpace")] = TL::SystemInformation::getSocInfo().freeSketchSpace;

	const JsonObject hardwareInfo = jsonDoc.createNestedObject(F("hardwareInfo"));
	hardwareInfo[F("hwVersion")] = TL::SystemInformation::getHardwareInfo().hwVersion;
	hardwareInfo[F("regulatorCount")] = TL::SystemInformation::getHardwareInfo().regulatorCount;
	hardwareInfo[F("regulatorVoltage")] = TL::SystemInformation::getHardwareInfo().regulatorVoltage;
	hardwareInfo[F("regulatorCurrentLimit")] = TL::SystemInformation::getHardwareInfo().regulatorCurrentLimit;
	hardwareInfo[F("regulatorCurrentDraw")] = TL::SystemInformation::getHardwareInfo().regulatorCurrentDraw;
	hardwareInfo[F("regulatorPowerLimit")] = TL::SystemInformation::getHardwareInfo().regulatorPowerLimit;
	hardwareInfo[F("regulatorPowerDraw")] = TL::SystemInformation::getHardwareInfo().regulatorPowerDraw;
	hardwareInfo[F("regulatorTemperature")] = TL::SystemInformation::getHardwareInfo().regulatorTemperature;
	hardwareInfo[F("fanSpeed")] = TL::SystemInformation::getHardwareInfo().fanSpeed;
	hardwareInfo[F("mpu6050")] = TL::SystemInformation::getHardwareInfo().mpu6050;
	hardwareInfo[F("ds18b20")] = TL::SystemInformation::getHardwareInfo().ds18b20;
	hardwareInfo[F("bh1750")] = TL::SystemInformation::getHardwareInfo().bh1750;
	hardwareInfo[F("audioUnit")] = TL::SystemInformation::getHardwareInfo().audioUnit;

	const JsonObject tlSystemInfo = jsonDoc.createNestedObject(F("tlSystemInfo"));
	tlSystemInfo[F("fps")] = TL::SystemInformation::getTesLightInfo().fps;
	tlSystemInfo[F("ledCount")] = TL::SystemInformation::getTesLightInfo().ledCount;

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::SystemInformationEndpoint::sendJsonDocument(200, F("Here is my current status."), jsonDoc);
}
