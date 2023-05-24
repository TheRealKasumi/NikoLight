/**
 * @file ResetEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to remotely reset the controller in case something is going wrong.
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