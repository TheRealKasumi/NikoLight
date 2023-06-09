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
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::SystemInformationEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("info/system")).c_str(), http_method::HTTP_GET, NL::SystemInformationEndpoint::getSystemInformation);
}

/**
 * @brief Return the system information/status to the client.
 */
void NL::SystemInformationEndpoint::getSystemInformation()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the system information/status."));
	if (!NL::SystemInformation::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight system information was not initialized. Can not access."));
		NL::SystemInformationEndpoint::sendSimpleResponse(500, F("The NikoLight system information was not initialized. Can not access."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);

	const JsonObject socInfo = jsonDoc.createNestedObject(F("socInfo"));
	socInfo[F("chipModel")] = NL::SystemInformation::getSocInfo().chipModel;
	socInfo[F("chipRevision")] = NL::SystemInformation::getSocInfo().chipRevision;
	socInfo[F("fwVersion")] = NL::SystemInformation::getSocInfo().fwVersion;
	socInfo[F("cpuCores")] = NL::SystemInformation::getSocInfo().cpuCores;
	socInfo[F("cpuClock")] = NL::SystemInformation::getSocInfo().cpuClock;
	socInfo[F("freeHeap")] = NL::SystemInformation::getSocInfo().freeHeap;
	socInfo[F("flashSize")] = NL::SystemInformation::getSocInfo().flashSize;
	socInfo[F("flashSpeed")] = NL::SystemInformation::getSocInfo().flashSpeed;
	socInfo[F("sketchSize")] = NL::SystemInformation::getSocInfo().sketchSize;
	socInfo[F("freeSketchSpace")] = NL::SystemInformation::getSocInfo().freeSketchSpace;

	const JsonObject hardwareInfo = jsonDoc.createNestedObject(F("hardwareInfo"));
	hardwareInfo[F("hwVersion")] = NL::SystemInformation::getHardwareInfo().hwVersion;
	hardwareInfo[F("regulatorCount")] = NL::SystemInformation::getHardwareInfo().regulatorCount;
	hardwareInfo[F("regulatorVoltage")] = NL::SystemInformation::getHardwareInfo().regulatorVoltage;
	hardwareInfo[F("regulatorCurrentLimit")] = NL::SystemInformation::getHardwareInfo().regulatorCurrentLimit;
	hardwareInfo[F("regulatorCurrentDraw")] = NL::SystemInformation::getHardwareInfo().regulatorCurrentDraw;
	hardwareInfo[F("regulatorPowerLimit")] = NL::SystemInformation::getHardwareInfo().regulatorPowerLimit;
	hardwareInfo[F("regulatorPowerDraw")] = NL::SystemInformation::getHardwareInfo().regulatorPowerDraw;
	hardwareInfo[F("regulatorTemperature")] = NL::SystemInformation::getHardwareInfo().regulatorTemperature;
	hardwareInfo[F("fanSpeed")] = NL::SystemInformation::getHardwareInfo().fanSpeed;
	hardwareInfo[F("mpu6050")] = NL::SystemInformation::getHardwareInfo().mpu6050;
	hardwareInfo[F("ds18b20")] = NL::SystemInformation::getHardwareInfo().ds18b20;
	hardwareInfo[F("bh1750")] = NL::SystemInformation::getHardwareInfo().bh1750;
	hardwareInfo[F("audioUnit")] = NL::SystemInformation::getHardwareInfo().audioUnit;

	const JsonObject tlSystemInfo = jsonDoc.createNestedObject(F("nlSystemInfo"));
	tlSystemInfo[F("fps")] = NL::SystemInformation::getNikoLightInfo().fps;
	tlSystemInfo[F("ledCount")] = NL::SystemInformation::getNikoLightInfo().ledCount;
	tlSystemInfo[F("hiddenLedCount")] = NL::SystemInformation::getNikoLightInfo().hiddenLedCount;

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::SystemInformationEndpoint::sendJsonDocument(200, F("Here is my current status."), jsonDoc);
}
