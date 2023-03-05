/**
 * @file Configuration.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TL::Configuration}.
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
#include "configuration/Configuration.h"

bool TL::Configuration::initialized = false;
FS *TL::Configuration::fileSystem;
String TL::Configuration::fileName;
uint16_t TL::Configuration::configurationVersion;
size_t TL::Configuration::activeProfile;
std::vector<TL::Configuration::Profile> TL::Configuration::profiles;
TL::Configuration::WiFiConfig TL::Configuration::wifiConfig;
TL::Configuration::MotionSensorCalibration TL::Configuration::motionSensorCalibration;
TL::Configuration::AudioUnitConfig TL::Configuration::audioUnitConfig;

/**
 * @brief Initialize the configuration.
 * @param fileSystem pointer to a {@link FS}
 * @param fileName file name of the configuration file
 */
void TL::Configuration::begin(FS *fileSystem, const String fileName)
{
	// Initialize
	TL::Configuration::initialized = true;
	TL::Configuration::fileSystem = fileSystem;
	TL::Configuration::fileName = fileName;
	TL::Configuration::configurationVersion = CONFIGURATION_FILE_VERSION;

	// Load default settings
	TL::Configuration::loadDefaults();
}

/**
 * @brief Deinitialize the configuration.
 */
void TL::Configuration::end()
{
	TL::Configuration::initialized = false;
	TL::Configuration::profiles.clear();
}

/**
 * @brief Return if the configuration was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::Configuration::isInitialized()
{
	return TL::Configuration::initialized;
}

/**
 * @brief Get the number of profiles currently stored in the configuration.
 * @return number of profiles currently stored in the configuration
 */
size_t TL::Configuration::getProfileCount()
{
	return TL::Configuration::profiles.size();
}

/**
 * @brief Get the profile name by the profile index.
 * @param profileIndex index of the porfile
 * @param profileName reference to a string holding the name
 * @return OK when the profile name was read
 * @return ERROR_OUT_OF_BOUNDS when the profile index is invalid
 */
TL::Configuration::Error TL::Configuration::getProfileNameByIndex(const size_t profileIndex, String &profileName)
{
	if (profileIndex >= TL::Configuration::profiles.size())
	{
		return TL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	profileName = TL::Configuration::profiles.at(profileIndex).name;
	return TL::Configuration::Error::OK;
}

/**
 * @brief Get a single profile from the configuration.
 * @param profileName name of the profile to get
 * @param profile reference to a variable holding the profile
 * @return OK when the profile was read
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
TL::Configuration::Error TL::Configuration::getProfile(const String &profileName, TL::Configuration::Profile &profile)
{
	size_t profileIndex;
	const TL::Configuration::Error findError = TL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError != TL::Configuration::Error::OK)
	{
		return findError;
	}

	profile = TL::Configuration::profiles.at(profileIndex);
	return TL::Configuration::Error::OK;
}

/**
 * @brief Create a new profile with default settings.
 * @param profileName name of the new profile
 * @return OK when the new profile was created
 * @return ERROR_TOO_MANY_PROFILES when the profile limited is reached
 * @return ERROR_PROFILE_NAME_EXISTS when the profile name already exists
 */
TL::Configuration::Error TL::Configuration::createProfile(const String &profileName)
{
	if (TL::Configuration::profiles.size() + 1 > CONFIGURATION_MAX_PROFILES)
	{
		return TL::Configuration::Error::ERROR_TOO_MANY_PROFILES;
	}

	size_t profileIndex;
	const TL::Configuration::Error findError = TL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError == TL::Configuration::Error::OK)
	{
		return TL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS;
	}

	TL::Configuration::Profile profile;
	profile.name = profileName;
	TL::Configuration::profiles.push_back(profile);
	TL::Configuration::loadProfileDefaults(TL::Configuration::profiles.size() - 1);
	return TL::Configuration::Error::OK;
}

/**
 * @brief Clone an existing profile and give it a new name.
 * @param sourceName name of the existing profile
 * @param destinationName name of the new profile
 * @return OK when the new profile was cloned
 * @return ERROR_TOO_MANY_PROFILES when the profile limited is reached
 * @return ERROR_PROFILE_NAME_EXISTS when the profile name already exists
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
TL::Configuration::Error TL::Configuration::cloneProfile(const String &sourceName, const String &destinationName)
{
	if (TL::Configuration::profiles.size() + 1 > CONFIGURATION_MAX_PROFILES)
	{
		return TL::Configuration::Error::ERROR_TOO_MANY_PROFILES;
	}

	size_t newProfileIndex;
	const TL::Configuration::Error findError = TL::Configuration::getProfileIndexByName(destinationName, newProfileIndex);
	if (findError == TL::Configuration::Error::OK)
	{
		return TL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS;
	}

	TL::Configuration::Profile existingProfile;
	const TL::Configuration::Error loadError = TL::Configuration::getProfile(sourceName, existingProfile);
	if (loadError != TL::Configuration::Error::OK)
	{
		return loadError;
	}

	existingProfile.name = destinationName;
	TL::Configuration::profiles.push_back(existingProfile);
	return TL::Configuration::Error::OK;
}

/**
 * @brief Rename a existing profile.
 * @param profileName name of the existing profile
 * @param newProfileName new name of the profile
 * @return OK when the profile was renamed
 * @return ERROR_PROFILE_NAME_EXISTS when the profile name already exists
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
TL::Configuration::Error TL::Configuration::renameProfile(const String &profileName, const String &newProfileName)
{
	size_t profileIndex;
	const TL::Configuration::Error findNewError = TL::Configuration::getProfileIndexByName(newProfileName, profileIndex);
	if (findNewError == TL::Configuration::Error::OK)
	{
		return TL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS;
	}

	const TL::Configuration::Error findOldError = TL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findOldError != TL::Configuration::Error::OK)
	{
		return TL::Configuration::Error::ERROR_PROFILE_NOT_FOUND;
	}

	TL::Configuration::profiles.at(profileIndex).name = newProfileName;
	return TL::Configuration::Error::OK;
}

/**
 * @brief Delete a profile by the profile name.
 * @param profileName name of the profile to delete
 * @return OK when the profile was delete
 * @return ERROR_PROFILE_IS_ACTIVE when the currently active profile is deleted
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
TL::Configuration::Error TL::Configuration::deleteProfile(const String &profileName)
{
	size_t profileIndex;
	const TL::Configuration::Error findError = TL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError != TL::Configuration::Error::OK)
	{
		return findError;
	}

	if (TL::Configuration::activeProfile == profileIndex)
	{
		return TL::Configuration::Error::ERROR_PROFILE_IS_ACTIVE;
	}
	else if (TL::Configuration::activeProfile > profileIndex)
	{
		TL::Configuration::activeProfile--;
	}

	TL::Configuration::profiles.erase(TL::Configuration::profiles.begin() + profileIndex);
	return TL::Configuration::Error::OK;
}

/**
 * @brief Get the currently active profile name.
 * @return name of the active profile
 */
String TL::Configuration::getActiveProfile()
{
	return TL::Configuration::profiles.at(TL::Configuration::activeProfile).name;
}

/**
 * @brief Set the active profile by the profile name.
 * @param profileName name of the profile to activate
 * @return OK when the active profile was set
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
TL::Configuration::Error TL::Configuration::setActiveProfile(const String &profileName)
{
	size_t profileIndex;
	const TL::Configuration::Error findError = TL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError != TL::Configuration::Error::OK)
	{
		return findError;
	}

	TL::Configuration::activeProfile = profileIndex;
	return TL::Configuration::Error::OK;
}

/**
 * @brief Get the system configuration.
 * @return system configuration
 */
TL::Configuration::SystemConfig TL::Configuration::getSystemConfig()
{
	return TL::Configuration::profiles.at(TL::Configuration::activeProfile).systemConfig;
}

/**
 * @brief Set the system configuration.
 * @param systemConfig new system configuration
 */
void TL::Configuration::setSystemConfig(TL::Configuration::SystemConfig &systemConfig)
{
	TL::Configuration::profiles.at(TL::Configuration::activeProfile).systemConfig = systemConfig;
}

/**
 * @brief Get the LED configuration for a zone by the zone index.
 * @param index index of the zone
 * @param ledConfig reference variable holding the LED configuration
 * @return OK when the configuration for the zone was read
 * @return ERROR_OUT_OF_BOUNDS when the zone index is out of bounds
 */
TL::Configuration::Error TL::Configuration::getLedConfig(const uint8_t zoneIndex, TL::Configuration::LedConfig &ledConfig)
{
	if (zoneIndex >= LED_NUM_ZONES)
	{
		return TL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	ledConfig = TL::Configuration::profiles.at(TL::Configuration::activeProfile).ledConfig[zoneIndex];
	return TL::Configuration::Error::OK;
}

/**
 * @brief Set the LED configuration for a zone by the zone index.
 * @param index index of the zone
 * @param ledConfig reference variable holding the LED configuration
 * @return OK when the configuration for the zone was updated
 * @return ERROR_OUT_OF_BOUNDS when the zone index is out of bounds
 */
TL::Configuration::Error TL::Configuration::setLedConfig(const uint8_t zoneIndex, const TL::Configuration::LedConfig &ledConfig)
{
	if (zoneIndex >= LED_NUM_ZONES)
	{
		return TL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	TL::Configuration::profiles.at(TL::Configuration::activeProfile).ledConfig[zoneIndex] = ledConfig;
	return TL::Configuration::Error::OK;
}

/**
 * @brief Get the WiFi configuration.
 * @return WiFi configuration
 */
TL::Configuration::WiFiConfig TL::Configuration::getWiFiConfig()
{
	return TL::Configuration::wifiConfig;
}

/**
 * @brief Set the WiFi configuration.
 * @param wifionfig new WiFi configuration
 */
void TL::Configuration::setWiFiConfig(TL::Configuration::WiFiConfig &wifiConfig)
{
	TL::Configuration::wifiConfig = wifiConfig;
}

/**
 * @brief Get the motion sensor calibration data.
 * @return calibration data of the motion sensor
 */
TL::Configuration::MotionSensorCalibration TL::Configuration::getMotionSensorCalibration()
{
	return TL::Configuration::motionSensorCalibration;
}

/**
 * @brief Set the motion sensor calibration data.
 * @param calibration new calibration data for the motion sensor
 */
void TL::Configuration::setMotionSensorCalibration(const TL::Configuration::MotionSensorCalibration &calibration)
{
	TL::Configuration::motionSensorCalibration = calibration;
}

/**
 * @brief Get the audio unit configuration.
 * @return audio unit configuration
 */
TL::Configuration::AudioUnitConfig TL::Configuration::getAudioUnitConfig()
{
	return TL::Configuration::audioUnitConfig;
}

/**
 * @brief Set the audio unit configuration;
 * @param audioUnitConfig new audio unit configuration
 */
void TL::Configuration::setAudioUnitConfig(const TL::Configuration::AudioUnitConfig &audioUnitConfig)
{
	TL::Configuration::audioUnitConfig = audioUnitConfig;
}

/**
 * @brief Get the UI configuration.
 * @return UI configuration
 */
TL::Configuration::UIConfiguration TL::Configuration::getUIConfiguration()
{
	return TL::Configuration::profiles.at(TL::Configuration::activeProfile).uiConfiguration;
}

/**
 * @brief Set the UI configuration.
 * @param uiConfiguration new UI configuration
 */
void TL::Configuration::setUIConfiguration(const TL::Configuration::UIConfiguration &uiConfiguration)
{
	TL::Configuration::profiles.at(TL::Configuration::activeProfile).uiConfiguration = uiConfiguration;
}

/**
 * @brief Load the default, profile independant settings.
 */
void TL::Configuration::loadDefaults()
{
	// Create a default profile
	TL::Configuration::activeProfile = 0;
	TL::Configuration::Profile defaultProfile;
	defaultProfile.name = F("Default Profile");
	TL::Configuration::profiles.clear();
	TL::Configuration::profiles.push_back(defaultProfile);
	TL::Configuration::loadProfileDefaults(TL::Configuration::activeProfile);

	// WiFi config
	TL::Configuration::wifiConfig.accessPointSsid = F(AP_DEFAULT_SSID);
	TL::Configuration::wifiConfig.accessPointPassword = F(AP_DEDAULT_PASSWORD);
	TL::Configuration::wifiConfig.accessPointChannel = AP_DEFAULT_CHANNEL;
	TL::Configuration::wifiConfig.accessPointHidden = AP_DEFAULT_HIDDEN;
	TL::Configuration::wifiConfig.accessPointMaxConnections = AP_DEFAULT_MAX_CONN;
	TL::Configuration::wifiConfig.wifiSsid = F(WIFI_DEFAULT_SSID);
	TL::Configuration::wifiConfig.wifiPassword = F(WIFI_DEFAULT_PASSWORD);

	// Motion sensor calibration
	TL::Configuration::motionSensorCalibration.accXRaw = 0;
	TL::Configuration::motionSensorCalibration.accYRaw = 0;
	TL::Configuration::motionSensorCalibration.accZRaw = 0;
	TL::Configuration::motionSensorCalibration.gyroXRaw = 0;
	TL::Configuration::motionSensorCalibration.gyroYRaw = 0;
	TL::Configuration::motionSensorCalibration.gyroZRaw = 0;
	TL::Configuration::motionSensorCalibration.accXG = 0.0f;
	TL::Configuration::motionSensorCalibration.accYG = 0.0f;
	TL::Configuration::motionSensorCalibration.accZG = 0.0f;
	TL::Configuration::motionSensorCalibration.gyroXDeg = 0.0f;
	TL::Configuration::motionSensorCalibration.gyroYDeg = 0.0f;
	TL::Configuration::motionSensorCalibration.gyroZDeg = 0.0f;

	// Audio unit configuration
	const uint16_t freqBands[AUDIO_UNIT_NUM_BANDS][2] = AUDIO_UNIT_DEFAULT_FREQ_BAND_INDEX;
	TL::Configuration::audioUnitConfig.noiseThreshold = AUDIO_UNIT_DEFAULT_NOISE_THESHOLD;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		TL::Configuration::audioUnitConfig.frequencyBandIndex[i] = std::make_pair(freqBands[i][0], freqBands[i][1]);
		TL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize = AUDIO_UNIT_DEFAULT_PD_HIST_SIZE;
		TL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold = AUDIO_UNIT_DEFAULT_PD_THRESHOLD;
		TL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence = AUDIO_UNIT_DEFAULT_PD_INFLUENCE;
		TL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate = AUDIO_UNIT_DEFAULT_PD_NOISE_GATE;
	}
}

/**
 * @brief Load the configuration from a binary file.
 * @return OK when the file was loaded
 * @return ERROR_FILE_OPEN when the file could not be opened
 * @return ERROR_FILE_READ when the file could not be read
 * @return ERROR_FILE_VERSION when the file version does not match
 * @return ERROR_FILE_HASH when the file hash doesn't match
 * @return ERROR_TOO_MANY_PROFILES when the configuration file contains too many profiles
 */
TL::Configuration::Error TL::Configuration::load()
{
	TL::BinaryFile file(TL::Configuration::fileSystem);
	const TL::BinaryFile::Error openError = file.open(TL::Configuration::fileName, FILE_READ);
	if (openError != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_OPEN;
	}

	// Read the file version
	uint16_t configFileVersion = 0;
	if (file.read(configFileVersion) != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_READ;
	}

	// Check the configuration file version
	if (configFileVersion != TL::Configuration::configurationVersion)
	{
		file.close();
		return TL::Configuration::Error::ERROR_FILE_VERSION;
	}

	// Read the number of profiles in the configuration file
	uint16_t profileCount;
	if (file.read(profileCount) != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_READ;
	}

	// Check the number of profiles
	if (profileCount > CONFIGURATION_MAX_PROFILES)
	{
		return TL::Configuration::Error::ERROR_TOO_MANY_PROFILES;
	}

	// Read the last active profile from the configuration file
	if (file.read(TL::Configuration::activeProfile) != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_READ;
	}

	// Read all profiles
	TL::Configuration::profiles.resize(profileCount);
	for (size_t i = 0; i < profileCount; i++)
	{
		// Get the profile
		TL::Configuration::Profile profile = TL::Configuration::profiles.at(i);

		// Profile name
		bool readError = false;
		readError = file.readString(profile.name) == TL::BinaryFile::Error::OK ? readError : true;

		// System config
		readError = file.read(profile.systemConfig.logLevel) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMode) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorThreshold) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMinAmbientBrightness) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMaxAmbientBrightness) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMinLedBrightness) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMaxLedBrightness) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorDuration) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.regulatorPowerLimit) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.regulatorHighTemperature) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.regulatorCutoffTemperature) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMode) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMinPwmValue) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMaxPwmValue) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMinTemperature) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMaxTemperature) == TL::BinaryFile::Error::OK ? readError : true;

		// LED config
		for (uint8_t j = 0; j < LED_NUM_ZONES; j++)
		{
			readError = file.read(profile.ledConfig[j].ledPin) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledCount) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].type) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].dataSource) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].speed) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].offset) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].brightness) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].reverse) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].fadeSpeed) == TL::BinaryFile::Error::OK ? readError : true;
			for (uint8_t k = 0; k < ANIMATOR_NUM_ANIMATION_SETTINGS; k++)
			{
				readError = file.read(profile.ledConfig[j].animationSettings[k]) == TL::BinaryFile::Error::OK ? readError : true;
			}
			readError = file.read(profile.ledConfig[j].ledVoltage) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledChannelCurrent[0]) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledChannelCurrent[1]) == TL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledChannelCurrent[2]) == TL::BinaryFile::Error::OK ? readError : true;
		}

		// UI configuration
		readError = file.readString(profile.uiConfiguration.language) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.readString(profile.uiConfiguration.theme) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.uiConfiguration.expertMode) == TL::BinaryFile::Error::OK ? readError : true;

		// Check for read errors
		if (readError)
		{
			file.close();
			return TL::Configuration::Error::ERROR_FILE_READ;
		}

		// Update the profile
		TL::Configuration::profiles.at(i) = profile;
	}

	// WiFi config
	bool readError = false;
	readError = file.readString(TL::Configuration::wifiConfig.accessPointSsid) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.readString(TL::Configuration::wifiConfig.accessPointPassword) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::wifiConfig.accessPointChannel) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::wifiConfig.accessPointHidden) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::wifiConfig.accessPointMaxConnections) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.readString(TL::Configuration::wifiConfig.wifiSsid) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.readString(TL::Configuration::wifiConfig.wifiPassword) == TL::BinaryFile::Error::OK ? readError : true;

	// Motion sensor calibration
	readError = file.read(TL::Configuration::motionSensorCalibration.accXRaw) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.accYRaw) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.accZRaw) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.gyroXRaw) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.gyroYRaw) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.gyroZRaw) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.accXG) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.accYG) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.accZG) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.gyroXDeg) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.gyroYDeg) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::motionSensorCalibration.gyroZDeg) == TL::BinaryFile::Error::OK ? readError : true;

	// Audio unit configuration
	readError = file.read(TL::Configuration::audioUnitConfig.noiseThreshold) == TL::BinaryFile::Error::OK ? readError : true;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		readError = file.read(TL::Configuration::audioUnitConfig.frequencyBandIndex[i].first) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::audioUnitConfig.frequencyBandIndex[i].second) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate) == TL::BinaryFile::Error::OK ? readError : true;
	}

	// Check for read errors
	if (readError)
	{
		file.close();
		return TL::Configuration::Error::ERROR_FILE_READ;
	}

	// Read the file hash
	uint16_t fileHash = 0;
	if (file.read(fileHash) != TL::BinaryFile::Error::OK)
	{
		file.close();
		return TL::Configuration::Error::ERROR_FILE_READ;
	}

	// Check the hash
	if (fileHash != TL::Configuration::getSimpleHash())
	{
		file.close();
		return TL::Configuration::Error::ERROR_FILE_HASH;
	}

	file.close();
	return TL::Configuration::Error::OK;
}

/**
 * @brief Save to configuration to a file.
 * @return OK when the file was saved
 * @return ERROR_FILE_OPEN when the file could not be opened
 * @return ERROR_FILE_WRITE when the file could not be written
 */
TL::Configuration::Error TL::Configuration::save()
{
	TL::BinaryFile file(TL::Configuration::fileSystem);
	const TL::BinaryFile::Error openError = file.open(TL::Configuration::fileName, FILE_WRITE);
	if (openError != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_OPEN;
	}

	// Write the configuration file version
	if (file.write(TL::Configuration::configurationVersion) != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_WRITE;
	}

	// Write the number of profiles
	if (file.write(static_cast<uint16_t>(TL::Configuration::profiles.size())) != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_WRITE;
	}

	// Write the last active profile
	if (file.write(TL::Configuration::activeProfile) != TL::BinaryFile::Error::OK)
	{
		return TL::Configuration::Error::ERROR_FILE_WRITE;
	}

	// Write all profiles to the configuration file
	for (size_t i = 0; i < TL::Configuration::profiles.size(); i++)
	{
		// Get the profile
		TL::Configuration::Profile profile = TL::Configuration::profiles.at(i);

		// Profile name
		bool writeError = false;
		writeError = file.writeString(profile.name) == TL::BinaryFile::Error::OK ? writeError : true;

		// System cofiguration
		writeError = file.write(static_cast<uint8_t>(profile.systemConfig.logLevel)) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(static_cast<uint8_t>(profile.systemConfig.lightSensorMode)) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorThreshold) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMinAmbientBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMaxAmbientBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMinLedBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMaxLedBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorDuration) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.regulatorPowerLimit) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.regulatorHighTemperature) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.regulatorCutoffTemperature) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMode) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMinPwmValue) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMaxPwmValue) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMinTemperature) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMaxTemperature) == TL::BinaryFile::Error::OK ? writeError : true;

		// LED configuration
		for (uint8_t j = 0; j < LED_NUM_ZONES; j++)
		{
			writeError = file.write(profile.ledConfig[j].ledPin) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledCount) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].type) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].dataSource) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].speed) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].offset) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].brightness) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].reverse) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].fadeSpeed) == TL::BinaryFile::Error::OK ? writeError : true;
			for (uint8_t k = 0; k < ANIMATOR_NUM_ANIMATION_SETTINGS; k++)
			{
				writeError = file.write(profile.ledConfig[j].animationSettings[k]) == TL::BinaryFile::Error::OK ? writeError : true;
			}
			writeError = file.write(profile.ledConfig[j].ledVoltage) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledChannelCurrent[0]) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledChannelCurrent[1]) == TL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledChannelCurrent[2]) == TL::BinaryFile::Error::OK ? writeError : true;
		}

		// UI configuration
		writeError = file.writeString(profile.uiConfiguration.language) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.writeString(profile.uiConfiguration.theme) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.uiConfiguration.expertMode) == TL::BinaryFile::Error::OK ? writeError : true;

		// Check for write errors
		if (writeError)
		{
			file.close();
			return TL::Configuration::Error::ERROR_FILE_WRITE;
		}
	}

	// WiFi configuration
	bool writeError = false;
	writeError = file.writeString(TL::Configuration::wifiConfig.accessPointSsid) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.writeString(TL::Configuration::wifiConfig.accessPointPassword) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::wifiConfig.accessPointChannel) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::wifiConfig.accessPointHidden) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::wifiConfig.accessPointMaxConnections) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.writeString(TL::Configuration::wifiConfig.wifiSsid) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.writeString(TL::Configuration::wifiConfig.wifiPassword) == TL::BinaryFile::Error::OK ? writeError : true;

	// Motion sensor calibration
	writeError = file.write(TL::Configuration::motionSensorCalibration.accXRaw) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.accYRaw) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.accZRaw) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.gyroXRaw) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.gyroYRaw) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.gyroZRaw) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.accXG) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.accYG) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.accZG) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.gyroXDeg) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.gyroYDeg) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::motionSensorCalibration.gyroZDeg) == TL::BinaryFile::Error::OK ? writeError : true;

	// Audio unit configuration
	writeError = file.write(TL::Configuration::audioUnitConfig.noiseThreshold) == TL::BinaryFile::Error::OK ? writeError : true;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		writeError = file.write(TL::Configuration::audioUnitConfig.frequencyBandIndex[i].first) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::audioUnitConfig.frequencyBandIndex[i].second) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate) == TL::BinaryFile::Error::OK ? writeError : true;
	}

	// Write the hash
	writeError = file.write(TL::Configuration::getSimpleHash()) == TL::BinaryFile::Error::OK ? writeError : true;

	file.close();
	return !writeError ? TL::Configuration::Error::OK : TL::Configuration::Error::ERROR_FILE_WRITE;
}

/**
 * @brief Load the default, profile depending settings for the currently selected profile.
 * @param profileIndex index of the profile to reset
 * @return OK when the profile was reset
 * @return ERROR_OUT_OF_BOUNDS when the profile index is invalid
 */
TL::Configuration::Error TL::Configuration::loadProfileDefaults(const size_t profileIndex)
{
	if (profileIndex >= TL::Configuration::profiles.size())
	{
		return TL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	// Get the currently active profile
	TL::Configuration::Profile profile = TL::Configuration::profiles.at(profileIndex);

	// System config
	profile.systemConfig.logLevel = LOG_DEFAULT_LEVEL;
	profile.systemConfig.lightSensorMode = LIGHT_SENSOR_DEFAULT_MODE;
	profile.systemConfig.lightSensorThreshold = LIGHT_SENSOR_DEFAULT_THRESHOLD;
	profile.systemConfig.lightSensorMinAmbientBrightness = LIGHT_SENSOR_DEFAULT_MIN_AMBIENT;
	profile.systemConfig.lightSensorMaxAmbientBrightness = LIGHT_SENSOR_DEFAULT_MAX_AMBIENT;
	profile.systemConfig.lightSensorMinLedBrightness = LIGHT_SENSOR_DEFAULT_MIN_LED;
	profile.systemConfig.lightSensorMaxLedBrightness = LIGHT_SENSOR_DEFAULT_MAX_LED;
	profile.systemConfig.lightSensorDuration = LIGHT_SENSOR_DEFAULT_DURATION;
	profile.systemConfig.regulatorPowerLimit = REGULATOR_DEFAULT_POWER_LIMIT * REGULATOR_COUNT;
	profile.systemConfig.regulatorHighTemperature = REGULATOR_DEFAULT_HIGH_TEMP;
	profile.systemConfig.regulatorCutoffTemperature = REGULATOR_DEFAULT_CUT_OFF_TEMP;
	profile.systemConfig.fanMode = FAN_DEFAULT_MODE;
	profile.systemConfig.fanMinPwmValue = FAN_DEFAULT_PWM_MIN;
	profile.systemConfig.fanMaxPwmValue = FAN_DEFAULT_PWM_MAX;
	profile.systemConfig.fanMinTemperature = FAN_DEFAULT_TEMP_MIN;
	profile.systemConfig.fanMaxTemperature = FAN_DEFAULT_TEMP_MAX;

	// LED config
	const uint8_t ledPins[LED_NUM_ZONES] = LED_DEFAULT_OUTPUT_PINS;
	const uint8_t ledCounts[LED_NUM_ZONES] = LED_DEFAULT_COUNTS;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		profile.ledConfig[i].ledPin = ledPins[i];
		profile.ledConfig[i].ledCount = ledCounts[i];
		profile.ledConfig[i].type = ANIMATOR_DEFAULT_TYPE;
		profile.ledConfig[i].dataSource = ANIMATOR_DEFAULT_DATA_SOURCE;
		profile.ledConfig[i].speed = ANIMATOR_DEFAULT_SPEED;
		profile.ledConfig[i].offset = ANIMATOR_DEFAULT_OFFSET;
		profile.ledConfig[i].brightness = ANIMATOR_DEFAULT_BRIGHTNESS;
		profile.ledConfig[i].reverse = ANIMATOR_DEFAULT_REVERSE;
		profile.ledConfig[i].fadeSpeed = ANIMATOR_DEFAULT_FADE_SPEED;
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			profile.ledConfig[i].animationSettings[j] = 0;
		}
		profile.ledConfig[i].ledVoltage = REGULATOR_VOLTAGE;
		profile.ledConfig[i].ledChannelCurrent[0] = LED_DEFAULT_CHANNEL_CURRENT;
		profile.ledConfig[i].ledChannelCurrent[1] = LED_DEFAULT_CHANNEL_CURRENT;
		profile.ledConfig[i].ledChannelCurrent[2] = LED_DEFAULT_CHANNEL_CURRENT;
	}

	// UI configuration
	profile.uiConfiguration.firmware = FW_VERSION;
	profile.uiConfiguration.language = UI_DEFAULT_LANGUAGE;
	profile.uiConfiguration.theme = UI_DEFAULT_THEME;
	profile.uiConfiguration.expertMode = UI_DEFAULT_EXPERT;

	// Update the currently active profile
	TL::Configuration::profiles.at(profileIndex) = profile;
	return TL::Configuration::Error::OK;
}

/**
 * @brief Get the index of a profile by the profile name.
 * @param profileName name of the profile
 * @param profileIndex reference variable holding the index
 * @return OK when the profile was found
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
TL::Configuration::Error TL::Configuration::getProfileIndexByName(const String &profileName, size_t &profileIndex)
{
	for (size_t i = 0; i < TL::Configuration::profiles.size(); i++)
	{
		if (TL::Configuration::profiles.at(i).name == profileName)
		{
			profileIndex = i;
			return TL::Configuration::Error::OK;
		}
	}
	return TL::Configuration::Error::ERROR_PROFILE_NOT_FOUND;
}

/**
 * @brief Calculate a very simple hash to check the config file.
 * @return uint16_t 2 byte hash
 */
uint16_t TL::Configuration::getSimpleHash()
{
	uint16_t hash = 7;

	// Configuration file version
	hash = hash * 31 + TL::Configuration::configurationVersion;

	// Configuration profile count
	hash = hash * 31 + TL::Configuration::profiles.size();

	// Configuration active profile
	hash = hash * 31 + TL::Configuration::activeProfile;

	// Run over all profiles
	for (size_t i = 0; i < TL::Configuration::profiles.size(); i++)
	{
		// Get the profile
		TL::Configuration::Profile profile = TL::Configuration::profiles.at(i);

		// Profile name
		hash = hash * 31 + TL::Configuration::getSimpleStringHash(profile.name);

		// System configuration
		hash = hash * 31 + profile.systemConfig.logLevel;
		hash = hash * 31 + profile.systemConfig.lightSensorMode;
		hash = hash * 31 + profile.systemConfig.lightSensorThreshold;
		hash = hash * 31 + profile.systemConfig.lightSensorMinAmbientBrightness;
		hash = hash * 31 + profile.systemConfig.lightSensorMaxAmbientBrightness;
		hash = hash * 31 + profile.systemConfig.lightSensorMinLedBrightness;
		hash = hash * 31 + profile.systemConfig.lightSensorMaxLedBrightness;
		hash = hash * 31 + profile.systemConfig.regulatorPowerLimit;
		hash = hash * 31 + profile.systemConfig.regulatorHighTemperature;
		hash = hash * 31 + profile.systemConfig.regulatorCutoffTemperature;
		hash = hash * 31 + profile.systemConfig.fanMode;
		hash = hash * 31 + profile.systemConfig.fanMinPwmValue;
		hash = hash * 31 + profile.systemConfig.fanMaxPwmValue;
		hash = hash * 31 + profile.systemConfig.fanMinTemperature;
		hash = hash * 31 + profile.systemConfig.fanMaxTemperature;

		// LED configuration
		for (uint8_t j = 0; j < LED_NUM_ZONES; j++)
		{
			hash = hash * 31 + profile.ledConfig[j].ledPin;
			hash = hash * 31 + profile.ledConfig[j].ledCount;
			hash = hash * 31 + profile.ledConfig[j].type;
			hash = hash * 31 + profile.ledConfig[j].dataSource;
			hash = hash * 31 + profile.ledConfig[j].speed;
			hash = hash * 31 + profile.ledConfig[j].offset;
			hash = hash * 31 + profile.ledConfig[j].brightness;
			hash = hash * 31 + profile.ledConfig[j].reverse;
			hash = hash * 31 + profile.ledConfig[j].fadeSpeed;
			for (uint8_t k = 0; k < ANIMATOR_NUM_ANIMATION_SETTINGS; k++)
			{
				hash = hash * 31 + profile.ledConfig[j].animationSettings[k];
			}
			hash = hash * 31 + profile.ledConfig[j].ledVoltage;
			hash = hash * 31 + profile.ledConfig[j].ledChannelCurrent[0];
			hash = hash * 31 + profile.ledConfig[j].ledChannelCurrent[1];
			hash = hash * 31 + profile.ledConfig[j].ledChannelCurrent[2];
		}

		// UI configuration
		hash = hash * 32 + TL::Configuration::getSimpleStringHash(profile.uiConfiguration.language);
		hash = hash * 32 + TL::Configuration::getSimpleStringHash(profile.uiConfiguration.theme);
		hash = hash * 32 + profile.uiConfiguration.expertMode;
	}

	// WiFi configuration
	hash = hash * 31 + TL::Configuration::getSimpleStringHash(TL::Configuration::wifiConfig.accessPointSsid);
	hash = hash * 31 + TL::Configuration::getSimpleStringHash(TL::Configuration::wifiConfig.accessPointPassword);
	hash = hash * 31 + TL::Configuration::wifiConfig.accessPointChannel;
	hash = hash * 31 + TL::Configuration::wifiConfig.accessPointHidden;
	hash = hash * 31 + TL::Configuration::wifiConfig.accessPointMaxConnections;
	hash = hash * 31 + TL::Configuration::getSimpleStringHash(TL::Configuration::wifiConfig.wifiSsid);
	hash = hash * 31 + TL::Configuration::getSimpleStringHash(TL::Configuration::wifiConfig.wifiPassword);

	// Motion sensor calibration
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.accXRaw;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.accYRaw;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.accZRaw;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.gyroXRaw;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.gyroYRaw;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.gyroZRaw;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.accXG;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.accYG;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.accZG;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.gyroXDeg;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.gyroYDeg;
	hash = hash * 31 + TL::Configuration::motionSensorCalibration.gyroZDeg;

	// Audio unit configuration
	hash = hash * 31 + TL::Configuration::audioUnitConfig.noiseThreshold;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		hash = hash * 31 + TL::Configuration::audioUnitConfig.frequencyBandIndex[i].first;
		hash = hash * 31 + TL::Configuration::audioUnitConfig.frequencyBandIndex[i].second;
		hash = hash * 31 + TL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize;
		hash = hash * 31 + TL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold;
		hash = hash * 31 + TL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence;
		hash = hash * 31 + TL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate;
	}

	return hash;
}

/**
 * @brief Create a very simple hash based on a string.
 * @param input input string
 * @return uint16_t 2 byte hash
 */
uint16_t TL::Configuration::getSimpleStringHash(const String input)
{
	uint16_t hash = 7;
	for (uint16_t i = 0; i < input.length(); i++)
	{
		hash = hash * 31 + input.charAt(i);
	}
	return hash;
}