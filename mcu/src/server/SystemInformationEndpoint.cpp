/**
 * @file SystemInformationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to get system information.
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
