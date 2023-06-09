/**
 * @file ResetEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to remotely reset the controller in case something is going wrong.
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
#include "server/ResetEndpoint.h"

FS *NL::ResetEndpoint::fileSystem = nullptr;

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::ResetEndpoint::begin(FS *_fileSystem)
{
	NL::ResetEndpoint::fileSystem = _fileSystem;
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("reset/soft")).c_str(), http_method::HTTP_POST, NL::ResetEndpoint::handleSoftReset);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("reset/hard")).c_str(), http_method::HTTP_POST, NL::ResetEndpoint::handleHardReset);
}

/**
 * @brief Handler function for the soft reset.
 */
void NL::ResetEndpoint::handleSoftReset()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to execute a soft reset."));
	NL::ResetEndpoint::sendSimpleResponse(200, F("I will reboot for you in 3 seconds."));
	NL::Updater::reboot(F("Soft Reset"), 3000);
}

/**
 * @brief Handler function for the hard reset. This also deletes the configuration to start with defaults.
 */
void NL::ResetEndpoint::handleHardReset()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to execute a hard reset."));
	NL::ResetEndpoint::sendSimpleResponse(200, F("I will reset my configuration and then reboot for you in 3 seconds."));

	if (!NL::ResetEndpoint::fileSystem->remove(CONFIGURATION_FILE_NAME))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to remove configuration. This might be normal if it was not saved before."));
	}
	else
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Configuration was reset."));
	}

	NL::Updater::reboot(F("Hard Reset"), 3000);
}