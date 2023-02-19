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
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::ProfileEndpoint::begin()
{
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile/active")).c_str(), http_method::HTTP_GET, TL::ProfileEndpoint::getActiveProfile);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile/active")).c_str(), http_method::HTTP_PATCH, TL::ProfileEndpoint::patchActiveProfile);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_GET, TL::ProfileEndpoint::getProfiles);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_POST, TL::ProfileEndpoint::postProfile);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_PUT, TL::ProfileEndpoint::cloneProfile);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/profile")).c_str(), http_method::HTTP_DELETE, TL::ProfileEndpoint::deleteProfile);
}

/**
 * @brief Return the currently active profile name
 */
void TL::ProfileEndpoint::getActiveProfile()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the active user profile."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	const JsonObject profile = jsonDoc.createNestedObject(F("profile"));
	profile[F("name")] = TL::Configuration::getActiveProfile();

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::ProfileEndpoint::sendJsonDocument(200, F("Here is my active profile."), jsonDoc);
}

/**
 * @brief Update the active user profile.
 */
void TL::ProfileEndpoint::patchActiveProfile()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the active user profile."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasHeader(F("content-type")) || TL::ProfileEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("There must be a valid json body."));
		return;
	}

	const String body = TL::ProfileEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!TL::ProfileEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("profile")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"profile\" object."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The json must contain a \"profile\" object."));
		return;
	}

	const JsonObject profile = jsonDoc[F("profile")].as<JsonObject>();
	if (!profile[F("name")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field must be of type \"string\"."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field must be of type \"string\"."));
		return;
	}

	const String profileName = profile[F("name")];
	if (!TL::ProfileEndpoint::validateProfileName(profileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field contains invalid characters."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field contains invalid characters."));
		return;
	}

	const TL::Configuration::Error setProfileError = TL::Configuration::setActiveProfile(profileName);
	if (setProfileError == TL::Configuration::Error::ERROR_PROFILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile was not found."));
		TL::ProfileEndpoint::sendSimpleResponse(404, F("The profile was not found."));
		return;
	}
	else if (setProfileError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set the active user profile"));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to set the active user profile"));
		return;
	}

	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	const TL::LedManager::Error ledManagerError = TL::LedManager::reloadAnimations();
	if (ledManagerError == TL::LedManager::Error::ERROR_INIT_LED_DRIVER)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialized LED driver for the current configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to initialized LED driver for the current configuration."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. One of the animator types is unknown."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. One of the animator types is unknown."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_INVALID_FSEQ)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_FILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		return;
	}
	else if (ledManagerError != TL::LedManager::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration because of unknown reason"));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to apply LED configuration because of unknown reason"));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, my configuration is updated."));
}

/**
 * @brief Return a list of all known profiles.
 */
void TL::ProfileEndpoint::getProfiles()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get a list of all known user profiles."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(4096);
	const JsonObject profile = jsonDoc.createNestedObject(F("profile"));
	const JsonArray profileArray = profile.createNestedArray(F("names"));

	for (size_t i = 0; i < TL::Configuration::getProfileCount(); i++)
	{
		String profileName;
		TL::Configuration::getProfileNameByIndex(i, profileName);
		profileArray.add(profileName);
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::ProfileEndpoint::sendJsonDocument(200, F("Here is your profile list."), jsonDoc);
}

/**
 * @brief Create a new profile.
 */
void TL::ProfileEndpoint::postProfile()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to create a new user profile."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasHeader(F("content-type")) || TL::ProfileEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("There must be a valid json body."));
		return;
	}

	const String body = TL::ProfileEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!TL::ProfileEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("profile")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"profile\" object."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The json must contain a \"profile\" object."));
		return;
	}

	const JsonObject profile = jsonDoc[F("profile")].as<JsonObject>();
	if (!profile[F("name")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field must be of type \"string\"."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field must be of type \"string\"."));
		return;
	}

	const String profileName = profile[F("name")];
	if (!TL::ProfileEndpoint::validateProfileName(profileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field contains invalid characters."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field contains invalid characters."));
		return;
	}

	const TL::Configuration::Error createError = TL::Configuration::createProfile(profileName);
	if (createError == TL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile name already exists."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The profile name already exists."));
		return;
	}
	else if (createError == TL::Configuration::Error::ERROR_TOO_MANY_PROFILES)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The limit of 50 profiles is reached."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The limit of 50 profiles is reached."));
		return;
	}
	else if (createError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create a new profile."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to create a new profile."));
		return;
	}

	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, I created a new profile for you."));
}

/**
 * @brief Clone an existing profile into a new one.
 */
void TL::ProfileEndpoint::cloneProfile()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to clone a user profile."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasHeader(F("content-type")) || TL::ProfileEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasArg(F("name")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The name of the source profile must be provided via the \"name\" parameter."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The name of the source profile must be provided via the \"name\" parameter."));
		return;
	}

	const String sourceName = TL::ProfileEndpoint::webServer->arg(F("name"));
	if (!TL::ProfileEndpoint::validateProfileName(sourceName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" parameter contains invalid characters."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" parameter contains invalid characters."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("There must be a valid json body."));
		return;
	}

	const String body = TL::ProfileEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!TL::ProfileEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("profile")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"profile\" object."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The json must contain a \"profile\" object."));
		return;
	}

	const JsonObject profile = jsonDoc[F("profile")].as<JsonObject>();
	if (!profile[F("name")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field must be of type \"string\"."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field must be of type \"string\"."));
		return;
	}

	const String cloneName = profile[F("name")];
	if (!TL::ProfileEndpoint::validateProfileName(cloneName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" field contains invalid characters."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" field contains invalid characters."));
		return;
	}

	const TL::Configuration::Error cloneError = TL::Configuration::cloneProfile(sourceName, cloneName);
	if (cloneError == TL::Configuration::Error::ERROR_PROFILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The source profile was not found."));
		TL::ProfileEndpoint::sendSimpleResponse(404, F("The source profile was not found."));
		return;
	}
	else if (cloneError == TL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The clone name already exists."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The clone name already exists."));
		return;
	}
	else if (cloneError == TL::Configuration::Error::ERROR_TOO_MANY_PROFILES)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The limit of 50 profiles is reached."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The limit of 50 profiles is reached."));
		return;
	}
	else if (cloneError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to clone the profile."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to clone the profile."));
		return;
	}

	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, I cloned the profile for you."));
}

/**
 * @brief Delete a profile.
 */
void TL::ProfileEndpoint::deleteProfile()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to delete a user profile."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::ProfileEndpoint::webServer->hasArg(F("name")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The name of the profile must be provided via the \"name\" parameter."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The name of the profile must be provided via the \"name\" parameter."));
		return;
	}

	const String profileName = TL::ProfileEndpoint::webServer->arg(F("name"));
	if (!TL::ProfileEndpoint::validateProfileName(profileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"name\" parameter contains invalid characters."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The \"name\" parameter contains invalid characters."));
		return;
	}

	const TL::Configuration::Error deleteError = TL::Configuration::deleteProfile(profileName);
	if (deleteError == TL::Configuration::Error::ERROR_PROFILE_IS_ACTIVE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile can not be deleted while it is active."));
		TL::ProfileEndpoint::sendSimpleResponse(400, F("The profile can not be deleted while it is active."));
		return;
	}
	else if (deleteError == TL::Configuration::Error::ERROR_PROFILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The profile was not found."));
		TL::ProfileEndpoint::sendSimpleResponse(404, F("The profile was not found."));
		return;
	}
	else if (deleteError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to delete profile."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to delete profile."));
		return;
	}

	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be opened."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration. The configuration file could not be written."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save configuration."));
		TL::ProfileEndpoint::sendSimpleResponse(500, F("Failed to save configuration."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::ProfileEndpoint::sendSimpleResponse(200, F("Oki, I deleted the profile for you."));
}

/**
 * @brief Validate a profile name.
 * @param profileName profile name to validate
 * @return true when valid
 * @return false when invalid
 */
bool TL::ProfileEndpoint::validateProfileName(const String &profileName)
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
