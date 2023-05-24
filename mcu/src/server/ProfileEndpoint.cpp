/**
 * @file ProfileEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to manage user profiles.
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
#include "server/ProfileEndpoint.h"

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::ProfileEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile/active")).c_str(), http_method::HTTP_GET, NL::ProfileEndpoint::getActiveProfile);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile/active")).c_str(), http_method::HTTP_PATCH, NL::ProfileEndpoint::patchActiveProfile);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_GET, NL::ProfileEndpoint::getProfiles);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_POST, NL::ProfileEndpoint::postProfile);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_PUT, NL::ProfileEndpoint::cloneProfile);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_DELETE, NL::ProfileEndpoint::deleteProfile);
}

/**
 * @brief Return the currently active profile name
 */
void NL::ProfileEndpoint::getActiveProfile()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the active user profile."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	const JsonObject profile = jsonDoc.createNestedObject(F("profile"));
	profile[F("name")] = NL::Configuration::getActiveProfile();

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::ProfileEndpoint::sendJsonDocument(200, F("Here is my active profile."), jsonDoc);
}

/**
 * @brief Update the active user profile.
 */
void NL::ProfileEndpoint::patchActiveProfile()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the active user profile."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasHeader(F("content-type")) || NL::ProfileEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("There must be a valid json body."));
		return;
	}

	const String body = NL::ProfileEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!NL::ProfileEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("profile")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"profile\" object."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The json must contain a \"profile\" object."));
		return;
	}

	const JsonObject profile = jsonDoc[F("profile")].as<JsonObject>();
	if (!profile[F("name")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field must be of type \"string\"."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field must be of type \"string\"."));
		return;
	}

	const String profileName = profile[F("name")];
	if (!NL::ProfileEndpoint::validateProfileName(profileName))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field contains invalid characters."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field contains invalid characters."));
		return;
	}

	const NL::Configuration::Error setProfileError = NL::Configuration::setActiveProfile(profileName);
	if (setProfileError == NL::Configuration::Error::ERROR_PROFILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile was not found."));
		NL::ProfileEndpoint::sendSimpleResponse(404, F("The profile was not found."));
		return;
	}
	else if (setProfileError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set the active user profile"));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to set the active user profile"));
		return;
	}

	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	const NL::LedManager::Error ledManagerError = NL::LedManager::reloadAnimations();
	if (ledManagerError == NL::LedManager::Error::ERROR_INIT_LED_DRIVER)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialized LED driver for the current configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to initialized LED driver for the current configuration."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. One of the animator types is unknown."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. One of the animator types is unknown."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_INVALID_FSEQ)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_FILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		return;
	}
	else if (ledManagerError != NL::LedManager::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration because of unknown reason"));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to apply LED configuration because of unknown reason"));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, my configuration is updated."));
}

/**
 * @brief Return a list of all known profiles.
 */
void NL::ProfileEndpoint::getProfiles()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get a list of all known user profiles."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(4096);
	const JsonObject profile = jsonDoc.createNestedObject(F("profile"));
	const JsonArray profileArray = profile.createNestedArray(F("names"));

	for (size_t i = 0; i < NL::Configuration::getProfileCount(); i++)
	{
		String profileName;
		NL::Configuration::getProfileNameByIndex(i, profileName);
		profileArray.add(profileName);
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::ProfileEndpoint::sendJsonDocument(200, F("Here is your profile list."), jsonDoc);
}

/**
 * @brief Create a new profile.
 */
void NL::ProfileEndpoint::postProfile()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to create a new user profile."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasHeader(F("content-type")) || NL::ProfileEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("There must be a valid json body."));
		return;
	}

	const String body = NL::ProfileEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!NL::ProfileEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("profile")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"profile\" object."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The json must contain a \"profile\" object."));
		return;
	}

	const JsonObject profile = jsonDoc[F("profile")].as<JsonObject>();
	if (!profile[F("name")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field must be of type \"string\"."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field must be of type \"string\"."));
		return;
	}

	const String profileName = profile[F("name")];
	if (!NL::ProfileEndpoint::validateProfileName(profileName))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field contains invalid characters."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field contains invalid characters."));
		return;
	}

	const NL::Configuration::Error createError = NL::Configuration::createProfile(profileName);
	if (createError == NL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile name already exists."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The profile name already exists."));
		return;
	}
	else if (createError == NL::Configuration::Error::ERROR_TOO_MANY_PROFILES)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The limit of 50 profiles is reached."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The limit of 50 profiles is reached."));
		return;
	}
	else if (createError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create a new profile."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to create a new profile."));
		return;
	}

	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, I created a new profile for you."));
}

/**
 * @brief Clone an existing profile into a new one.
 */
void NL::ProfileEndpoint::cloneProfile()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to clone a user profile."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasHeader(F("content-type")) || NL::ProfileEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasArg(F("name")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The name of the source profile must be provided via the \"name\" parameter."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The name of the source profile must be provided via the \"name\" parameter."));
		return;
	}

	const String sourceName = NL::ProfileEndpoint::webServer->arg(F("name"));
	if (!NL::ProfileEndpoint::validateProfileName(sourceName))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" parameter contains invalid characters."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" parameter contains invalid characters."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("There must be a valid json body."));
		return;
	}

	const String body = NL::ProfileEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!NL::ProfileEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("profile")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"profile\" object."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The json must contain a \"profile\" object."));
		return;
	}

	const JsonObject profile = jsonDoc[F("profile")].as<JsonObject>();
	if (!profile[F("name")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field must be of type \"string\"."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field must be of type \"string\"."));
		return;
	}

	const String cloneName = profile[F("name")];
	if (!NL::ProfileEndpoint::validateProfileName(cloneName))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field contains invalid characters."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field contains invalid characters."));
		return;
	}

	const NL::Configuration::Error cloneError = NL::Configuration::cloneProfile(sourceName, cloneName);
	if (cloneError == NL::Configuration::Error::ERROR_PROFILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The source profile was not found."));
		NL::ProfileEndpoint::sendSimpleResponse(404, F("The source profile was not found."));
		return;
	}
	else if (cloneError == NL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The clone name already exists."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The clone name already exists."));
		return;
	}
	else if (cloneError == NL::Configuration::Error::ERROR_TOO_MANY_PROFILES)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The limit of 50 profiles is reached."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The limit of 50 profiles is reached."));
		return;
	}
	else if (cloneError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to clone the profile."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to clone the profile."));
		return;
	}

	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, I cloned the profile for you."));
}

/**
 * @brief Delete a profile.
 */
void NL::ProfileEndpoint::deleteProfile()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to delete a user profile."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::ProfileEndpoint::webServer->hasArg(F("name")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The name of the profile must be provided via the \"name\" parameter."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The name of the profile must be provided via the \"name\" parameter."));
		return;
	}

	const String profileName = NL::ProfileEndpoint::webServer->arg(F("name"));
	if (!NL::ProfileEndpoint::validateProfileName(profileName))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" parameter contains invalid characters."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" parameter contains invalid characters."));
		return;
	}

	const NL::Configuration::Error deleteError = NL::Configuration::deleteProfile(profileName);
	if (deleteError == NL::Configuration::Error::ERROR_PROFILE_IS_ACTIVE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile can not be deleted while it is active."));
		NL::ProfileEndpoint::sendSimpleResponse(400, F("The profile can not be deleted while it is active."));
		return;
	}
	else if (deleteError == NL::Configuration::Error::ERROR_PROFILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile was not found."));
		NL::ProfileEndpoint::sendSimpleResponse(404, F("The profile was not found."));
		return;
	}
	else if (deleteError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to delete profile."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to delete profile."));
		return;
	}

	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		NL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, I deleted the profile for you."));
}

/**
 * @brief Validate a profile name.
 * @param profileName profile name to validate
 * @return true when valid
 * @return false when invalid
 */
bool NL::ProfileEndpoint::validateProfileName(const String &profileName)
{
	if (profileName.length() < 3 || profileName.length() > 24)
	{
		return false;
	}

	const String validChars = F("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ÄÖÜäöüß+-*/#_<>!?");
	for (uint8_t i = 0; i < profileName.length(); i++)
	{
		if (validChars.indexOf(profileName.charAt(i)) == -1)
		{
			return false;
		}
	}

	return true;
}
