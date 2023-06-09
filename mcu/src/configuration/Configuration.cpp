/**
 * @file Configuration.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link NL::Configuration}.
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
#include "configuration/Configuration.h"

bool NL::Configuration::initialized = false;
FS *NL::Configuration::fileSystem;
String NL::Configuration::fileName;
uint16_t NL::Configuration::configurationVersion;
size_t NL::Configuration::activeProfile;
std::vector<NL::Configuration::Profile> NL::Configuration::profiles;
NL::Configuration::WiFiConfig NL::Configuration::wifiConfig;
NL::Configuration::MotionSensorCalibration NL::Configuration::motionSensorCalibration;
NL::Configuration::AudioUnitConfig NL::Configuration::audioUnitConfig;

/**
 * @brief Initialize the configuration.
 * @param fileSystem pointer to a {@link FS}
 * @param fileName file name of the configuration file
 */
void NL::Configuration::begin(FS *fileSystem, const String fileName)
{
	// Initialize
	NL::Configuration::initialized = true;
	NL::Configuration::fileSystem = fileSystem;
	NL::Configuration::fileName = fileName;
	NL::Configuration::configurationVersion = CONFIGURATION_FILE_VERSION;

	// Load default settings
	NL::Configuration::loadDefaults();
}

/**
 * @brief Deinitialize the configuration.
 */
void NL::Configuration::end()
{
	NL::Configuration::initialized = false;
	NL::Configuration::profiles.clear();
}

/**
 * @brief Return if the configuration was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::Configuration::isInitialized()
{
	return NL::Configuration::initialized;
}

/**
 * @brief Get the number of profiles currently stored in the configuration.
 * @return number of profiles currently stored in the configuration
 */
size_t NL::Configuration::getProfileCount()
{
	return NL::Configuration::profiles.size();
}

/**
 * @brief Get the profile name by the profile index.
 * @param profileIndex index of the porfile
 * @param profileName reference to a string holding the name
 * @return OK when the profile name was read
 * @return ERROR_OUT_OF_BOUNDS when the profile index is invalid
 */
NL::Configuration::Error NL::Configuration::getProfileNameByIndex(const size_t profileIndex, String &profileName)
{
	if (profileIndex >= NL::Configuration::profiles.size())
	{
		return NL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	profileName = NL::Configuration::profiles.at(profileIndex).name;
	return NL::Configuration::Error::OK;
}

/**
 * @brief Get a single profile from the configuration.
 * @param profileName name of the profile to get
 * @param profile reference to a variable holding the profile
 * @return OK when the profile was read
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
NL::Configuration::Error NL::Configuration::getProfile(const String &profileName, NL::Configuration::Profile &profile)
{
	size_t profileIndex;
	const NL::Configuration::Error findError = NL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError != NL::Configuration::Error::OK)
	{
		return findError;
	}

	profile = NL::Configuration::profiles.at(profileIndex);
	return NL::Configuration::Error::OK;
}

/**
 * @brief Create a new profile with default settings.
 * @param profileName name of the new profile
 * @return OK when the new profile was created
 * @return ERROR_TOO_MANY_PROFILES when the profile limited is reached
 * @return ERROR_PROFILE_NAME_EXISTS when the profile name already exists
 */
NL::Configuration::Error NL::Configuration::createProfile(const String &profileName)
{
	if (NL::Configuration::profiles.size() + 1 > CONFIGURATION_MAX_PROFILES)
	{
		return NL::Configuration::Error::ERROR_TOO_MANY_PROFILES;
	}

	size_t profileIndex;
	const NL::Configuration::Error findError = NL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError == NL::Configuration::Error::OK)
	{
		return NL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS;
	}

	NL::Configuration::Profile profile;
	profile.name = profileName;
	NL::Configuration::profiles.push_back(profile);
	NL::Configuration::loadProfileDefaults(NL::Configuration::profiles.size() - 1);
	return NL::Configuration::Error::OK;
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
NL::Configuration::Error NL::Configuration::cloneProfile(const String &sourceName, const String &destinationName)
{
	if (NL::Configuration::profiles.size() + 1 > CONFIGURATION_MAX_PROFILES)
	{
		return NL::Configuration::Error::ERROR_TOO_MANY_PROFILES;
	}

	size_t newProfileIndex;
	const NL::Configuration::Error findError = NL::Configuration::getProfileIndexByName(destinationName, newProfileIndex);
	if (findError == NL::Configuration::Error::OK)
	{
		return NL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS;
	}

	NL::Configuration::Profile existingProfile;
	const NL::Configuration::Error loadError = NL::Configuration::getProfile(sourceName, existingProfile);
	if (loadError != NL::Configuration::Error::OK)
	{
		return loadError;
	}

	existingProfile.name = destinationName;
	NL::Configuration::profiles.push_back(existingProfile);
	return NL::Configuration::Error::OK;
}

/**
 * @brief Rename a existing profile.
 * @param profileName name of the existing profile
 * @param newProfileName new name of the profile
 * @return OK when the profile was renamed
 * @return ERROR_PROFILE_NAME_EXISTS when the profile name already exists
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
NL::Configuration::Error NL::Configuration::renameProfile(const String &profileName, const String &newProfileName)
{
	size_t profileIndex;
	const NL::Configuration::Error findNewError = NL::Configuration::getProfileIndexByName(newProfileName, profileIndex);
	if (findNewError == NL::Configuration::Error::OK)
	{
		return NL::Configuration::Error::ERROR_PROFILE_NAME_EXISTS;
	}

	const NL::Configuration::Error findOldError = NL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findOldError != NL::Configuration::Error::OK)
	{
		return NL::Configuration::Error::ERROR_PROFILE_NOT_FOUND;
	}

	NL::Configuration::profiles.at(profileIndex).name = newProfileName;
	return NL::Configuration::Error::OK;
}

/**
 * @brief Delete a profile by the profile name.
 * @param profileName name of the profile to delete
 * @return OK when the profile was delete
 * @return ERROR_PROFILE_IS_ACTIVE when the currently active profile is deleted
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
NL::Configuration::Error NL::Configuration::deleteProfile(const String &profileName)
{
	size_t profileIndex;
	const NL::Configuration::Error findError = NL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError != NL::Configuration::Error::OK)
	{
		return findError;
	}

	if (NL::Configuration::activeProfile == profileIndex)
	{
		return NL::Configuration::Error::ERROR_PROFILE_IS_ACTIVE;
	}
	else if (NL::Configuration::activeProfile > profileIndex)
	{
		NL::Configuration::activeProfile--;
	}

	NL::Configuration::profiles.erase(NL::Configuration::profiles.begin() + profileIndex);
	return NL::Configuration::Error::OK;
}

/**
 * @brief Get the currently active profile name.
 * @return name of the active profile
 */
String NL::Configuration::getActiveProfile()
{
	return NL::Configuration::profiles.at(NL::Configuration::activeProfile).name;
}

/**
 * @brief Set the active profile by the profile name.
 * @param profileName name of the profile to activate
 * @return OK when the active profile was set
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
NL::Configuration::Error NL::Configuration::setActiveProfile(const String &profileName)
{
	size_t profileIndex;
	const NL::Configuration::Error findError = NL::Configuration::getProfileIndexByName(profileName, profileIndex);
	if (findError != NL::Configuration::Error::OK)
	{
		return findError;
	}

	NL::Configuration::activeProfile = profileIndex;
	return NL::Configuration::Error::OK;
}

/**
 * @brief Get the system configuration.
 * @return system configuration
 */
NL::Configuration::SystemConfig NL::Configuration::getSystemConfig()
{
	return NL::Configuration::profiles.at(NL::Configuration::activeProfile).systemConfig;
}

/**
 * @brief Set the system configuration.
 * @param systemConfig new system configuration
 */
void NL::Configuration::setSystemConfig(NL::Configuration::SystemConfig &systemConfig)
{
	NL::Configuration::profiles.at(NL::Configuration::activeProfile).systemConfig = systemConfig;
}

/**
 * @brief Get the LED configuration for a zone by the zone index.
 * @param index index of the zone
 * @param ledConfig reference variable holding the LED configuration
 * @return OK when the configuration for the zone was read
 * @return ERROR_OUT_OF_BOUNDS when the zone index is out of bounds
 */
NL::Configuration::Error NL::Configuration::getLedConfig(const uint8_t zoneIndex, NL::Configuration::LedConfig &ledConfig)
{
	if (zoneIndex >= LED_NUM_ZONES)
	{
		return NL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	ledConfig = NL::Configuration::profiles.at(NL::Configuration::activeProfile).ledConfig[zoneIndex];
	return NL::Configuration::Error::OK;
}

/**
 * @brief Set the LED configuration for a zone by the zone index.
 * @param index index of the zone
 * @param ledConfig reference variable holding the LED configuration
 * @return OK when the configuration for the zone was updated
 * @return ERROR_OUT_OF_BOUNDS when the zone index is out of bounds
 */
NL::Configuration::Error NL::Configuration::setLedConfig(const uint8_t zoneIndex, const NL::Configuration::LedConfig &ledConfig)
{
	if (zoneIndex >= LED_NUM_ZONES)
	{
		return NL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	NL::Configuration::profiles.at(NL::Configuration::activeProfile).ledConfig[zoneIndex] = ledConfig;
	return NL::Configuration::Error::OK;
}

/**
 * @brief Get the WiFi configuration.
 * @return WiFi configuration
 */
NL::Configuration::WiFiConfig NL::Configuration::getWiFiConfig()
{
	return NL::Configuration::wifiConfig;
}

/**
 * @brief Set the WiFi configuration.
 * @param wifionfig new WiFi configuration
 */
void NL::Configuration::setWiFiConfig(NL::Configuration::WiFiConfig &wifiConfig)
{
	NL::Configuration::wifiConfig = wifiConfig;
}

/**
 * @brief Get the motion sensor calibration data.
 * @return calibration data of the motion sensor
 */
NL::Configuration::MotionSensorCalibration NL::Configuration::getMotionSensorCalibration()
{
	return NL::Configuration::motionSensorCalibration;
}

/**
 * @brief Set the motion sensor calibration data.
 * @param calibration new calibration data for the motion sensor
 */
void NL::Configuration::setMotionSensorCalibration(const NL::Configuration::MotionSensorCalibration &calibration)
{
	NL::Configuration::motionSensorCalibration = calibration;
}

/**
 * @brief Get the audio unit configuration.
 * @return audio unit configuration
 */
NL::Configuration::AudioUnitConfig NL::Configuration::getAudioUnitConfig()
{
	return NL::Configuration::audioUnitConfig;
}

/**
 * @brief Set the audio unit configuration;
 * @param audioUnitConfig new audio unit configuration
 */
void NL::Configuration::setAudioUnitConfig(const NL::Configuration::AudioUnitConfig &audioUnitConfig)
{
	NL::Configuration::audioUnitConfig = audioUnitConfig;
}

/**
 * @brief Get the UI configuration.
 * @return UI configuration
 */
NL::Configuration::UIConfiguration NL::Configuration::getUIConfiguration()
{
	return NL::Configuration::profiles.at(NL::Configuration::activeProfile).uiConfiguration;
}

/**
 * @brief Set the UI configuration.
 * @param uiConfiguration new UI configuration
 */
void NL::Configuration::setUIConfiguration(const NL::Configuration::UIConfiguration &uiConfiguration)
{
	NL::Configuration::profiles.at(NL::Configuration::activeProfile).uiConfiguration = uiConfiguration;
}

/**
 * @brief Load the default, profile independant settings.
 */
void NL::Configuration::loadDefaults()
{
	// Create a default profile
	NL::Configuration::activeProfile = 0;
	NL::Configuration::Profile defaultProfile;
	defaultProfile.name = F("Default Profile");
	NL::Configuration::profiles.clear();
	NL::Configuration::profiles.push_back(defaultProfile);
	NL::Configuration::loadProfileDefaults(NL::Configuration::activeProfile);

	// WiFi config
	NL::Configuration::wifiConfig.accessPointSsid = F(AP_DEFAULT_SSID);
	NL::Configuration::wifiConfig.accessPointPassword = F(AP_DEDAULT_PASSWORD);
	NL::Configuration::wifiConfig.accessPointChannel = AP_DEFAULT_CHANNEL;
	NL::Configuration::wifiConfig.accessPointHidden = AP_DEFAULT_HIDDEN;
	NL::Configuration::wifiConfig.accessPointMaxConnections = AP_DEFAULT_MAX_CONN;
	NL::Configuration::wifiConfig.wifiSsid = F(WIFI_DEFAULT_SSID);
	NL::Configuration::wifiConfig.wifiPassword = F(WIFI_DEFAULT_PASSWORD);

	// Motion sensor calibration
	NL::Configuration::motionSensorCalibration.accXRaw = 0;
	NL::Configuration::motionSensorCalibration.accYRaw = 0;
	NL::Configuration::motionSensorCalibration.accZRaw = 0;
	NL::Configuration::motionSensorCalibration.gyroXRaw = 0;
	NL::Configuration::motionSensorCalibration.gyroYRaw = 0;
	NL::Configuration::motionSensorCalibration.gyroZRaw = 0;
	NL::Configuration::motionSensorCalibration.accXG = 0.0f;
	NL::Configuration::motionSensorCalibration.accYG = 0.0f;
	NL::Configuration::motionSensorCalibration.accZG = 0.0f;
	NL::Configuration::motionSensorCalibration.gyroXDeg = 0.0f;
	NL::Configuration::motionSensorCalibration.gyroYDeg = 0.0f;
	NL::Configuration::motionSensorCalibration.gyroZDeg = 0.0f;

	// Audio unit configuration
	const uint16_t freqBands[AUDIO_UNIT_NUM_BANDS][2] = AUDIO_UNIT_DEFAULT_FREQ_BAND_INDEX;
	NL::Configuration::audioUnitConfig.noiseThreshold = AUDIO_UNIT_DEFAULT_NOISE_THESHOLD;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		NL::Configuration::audioUnitConfig.frequencyBandIndex[i] = std::make_pair(freqBands[i][0], freqBands[i][1]);
		NL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize = AUDIO_UNIT_DEFAULT_PD_HIST_SIZE;
		NL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold = AUDIO_UNIT_DEFAULT_PD_THRESHOLD;
		NL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence = AUDIO_UNIT_DEFAULT_PD_INFLUENCE;
		NL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate = AUDIO_UNIT_DEFAULT_PD_NOISE_GATE;
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
NL::Configuration::Error NL::Configuration::load()
{
	NL::BinaryFile file(NL::Configuration::fileSystem);
	const NL::BinaryFile::Error openError = file.open(NL::Configuration::fileName, FILE_READ);
	if (openError != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_OPEN;
	}

	// Read the file version
	uint16_t configFileVersion = 0;
	if (file.read(configFileVersion) != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_READ;
	}

	// Check the configuration file version
	if (configFileVersion != NL::Configuration::configurationVersion)
	{
		file.close();
		return NL::Configuration::Error::ERROR_FILE_VERSION;
	}

	// Read the number of profiles in the configuration file
	uint16_t profileCount;
	if (file.read(profileCount) != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_READ;
	}

	// Check the number of profiles
	if (profileCount > CONFIGURATION_MAX_PROFILES)
	{
		return NL::Configuration::Error::ERROR_TOO_MANY_PROFILES;
	}

	// Read the last active profile from the configuration file
	if (file.read(NL::Configuration::activeProfile) != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_READ;
	}

	// Read all profiles
	NL::Configuration::profiles.resize(profileCount);
	for (size_t i = 0; i < profileCount; i++)
	{
		// Get the profile
		NL::Configuration::Profile profile = NL::Configuration::profiles.at(i);

		// Profile name
		bool readError = false;
		readError = file.readString(profile.name) == NL::BinaryFile::Error::OK ? readError : true;

		// System config
		readError = file.read(profile.systemConfig.logLevel) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMode) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorThreshold) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMinAmbientBrightness) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMaxAmbientBrightness) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMinLedBrightness) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorMaxLedBrightness) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.lightSensorDuration) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.regulatorPowerLimit) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.regulatorHighTemperature) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.regulatorCutoffTemperature) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMode) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMinPwmValue) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMaxPwmValue) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMinTemperature) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.systemConfig.fanMaxTemperature) == NL::BinaryFile::Error::OK ? readError : true;

		// LED config
		for (uint8_t j = 0; j < LED_NUM_ZONES; j++)
		{
			readError = file.read(profile.ledConfig[j].ledPin) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledCount) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].type) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].dataSource) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].speed) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].offset) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].brightness) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].reverse) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].fadeSpeed) == NL::BinaryFile::Error::OK ? readError : true;
			for (uint8_t k = 0; k < ANIMATOR_NUM_ANIMATION_SETTINGS; k++)
			{
				readError = file.read(profile.ledConfig[j].animationSettings[k]) == NL::BinaryFile::Error::OK ? readError : true;
			}
			readError = file.read(profile.ledConfig[j].ledVoltage) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledChannelCurrent[0]) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledChannelCurrent[1]) == NL::BinaryFile::Error::OK ? readError : true;
			readError = file.read(profile.ledConfig[j].ledChannelCurrent[2]) == NL::BinaryFile::Error::OK ? readError : true;
		}

		// UI configuration
		readError = file.readString(profile.uiConfiguration.language) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.readString(profile.uiConfiguration.theme) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(profile.uiConfiguration.expertMode) == NL::BinaryFile::Error::OK ? readError : true;

		// Check for read errors
		if (readError)
		{
			file.close();
			return NL::Configuration::Error::ERROR_FILE_READ;
		}

		// Update the profile
		NL::Configuration::profiles.at(i) = profile;
	}

	// WiFi config
	bool readError = false;
	readError = file.readString(NL::Configuration::wifiConfig.accessPointSsid) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.readString(NL::Configuration::wifiConfig.accessPointPassword) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::wifiConfig.accessPointChannel) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::wifiConfig.accessPointHidden) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::wifiConfig.accessPointMaxConnections) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.readString(NL::Configuration::wifiConfig.wifiSsid) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.readString(NL::Configuration::wifiConfig.wifiPassword) == NL::BinaryFile::Error::OK ? readError : true;

	// Motion sensor calibration
	readError = file.read(NL::Configuration::motionSensorCalibration.accXRaw) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.accYRaw) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.accZRaw) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.gyroXRaw) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.gyroYRaw) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.gyroZRaw) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.accXG) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.accYG) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.accZG) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.gyroXDeg) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.gyroYDeg) == NL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(NL::Configuration::motionSensorCalibration.gyroZDeg) == NL::BinaryFile::Error::OK ? readError : true;

	// Audio unit configuration
	readError = file.read(NL::Configuration::audioUnitConfig.noiseThreshold) == NL::BinaryFile::Error::OK ? readError : true;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		readError = file.read(NL::Configuration::audioUnitConfig.frequencyBandIndex[i].first) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(NL::Configuration::audioUnitConfig.frequencyBandIndex[i].second) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence) == NL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate) == NL::BinaryFile::Error::OK ? readError : true;
	}

	// Check for read errors
	if (readError)
	{
		file.close();
		return NL::Configuration::Error::ERROR_FILE_READ;
	}

	// Read the file hash
	uint16_t fileHash = 0;
	if (file.read(fileHash) != NL::BinaryFile::Error::OK)
	{
		file.close();
		return NL::Configuration::Error::ERROR_FILE_READ;
	}

	// Check the hash
	if (fileHash != NL::Configuration::getSimpleHash())
	{
		file.close();
		return NL::Configuration::Error::ERROR_FILE_HASH;
	}

	file.close();
	return NL::Configuration::Error::OK;
}

/**
 * @brief Save to configuration to a file.
 * @return OK when the file was saved
 * @return ERROR_FILE_OPEN when the file could not be opened
 * @return ERROR_FILE_WRITE when the file could not be written
 */
NL::Configuration::Error NL::Configuration::save()
{
	NL::BinaryFile file(NL::Configuration::fileSystem);
	const NL::BinaryFile::Error openError = file.open(NL::Configuration::fileName, FILE_WRITE);
	if (openError != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_OPEN;
	}

	// Write the configuration file version
	if (file.write(NL::Configuration::configurationVersion) != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_WRITE;
	}

	// Write the number of profiles
	if (file.write(static_cast<uint16_t>(NL::Configuration::profiles.size())) != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_WRITE;
	}

	// Write the last active profile
	if (file.write(NL::Configuration::activeProfile) != NL::BinaryFile::Error::OK)
	{
		return NL::Configuration::Error::ERROR_FILE_WRITE;
	}

	// Write all profiles to the configuration file
	for (size_t i = 0; i < NL::Configuration::profiles.size(); i++)
	{
		// Get the profile
		NL::Configuration::Profile profile = NL::Configuration::profiles.at(i);

		// Profile name
		bool writeError = false;
		writeError = file.writeString(profile.name) == NL::BinaryFile::Error::OK ? writeError : true;

		// System cofiguration
		writeError = file.write(static_cast<uint8_t>(profile.systemConfig.logLevel)) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(static_cast<uint8_t>(profile.systemConfig.lightSensorMode)) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorThreshold) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMinAmbientBrightness) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMaxAmbientBrightness) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMinLedBrightness) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorMaxLedBrightness) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.lightSensorDuration) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.regulatorPowerLimit) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.regulatorHighTemperature) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.regulatorCutoffTemperature) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMode) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMinPwmValue) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMaxPwmValue) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMinTemperature) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.systemConfig.fanMaxTemperature) == NL::BinaryFile::Error::OK ? writeError : true;

		// LED configuration
		for (uint8_t j = 0; j < LED_NUM_ZONES; j++)
		{
			writeError = file.write(profile.ledConfig[j].ledPin) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledCount) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].type) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].dataSource) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].speed) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].offset) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].brightness) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].reverse) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].fadeSpeed) == NL::BinaryFile::Error::OK ? writeError : true;
			for (uint8_t k = 0; k < ANIMATOR_NUM_ANIMATION_SETTINGS; k++)
			{
				writeError = file.write(profile.ledConfig[j].animationSettings[k]) == NL::BinaryFile::Error::OK ? writeError : true;
			}
			writeError = file.write(profile.ledConfig[j].ledVoltage) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledChannelCurrent[0]) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledChannelCurrent[1]) == NL::BinaryFile::Error::OK ? writeError : true;
			writeError = file.write(profile.ledConfig[j].ledChannelCurrent[2]) == NL::BinaryFile::Error::OK ? writeError : true;
		}

		// UI configuration
		writeError = file.writeString(profile.uiConfiguration.language) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.writeString(profile.uiConfiguration.theme) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(profile.uiConfiguration.expertMode) == NL::BinaryFile::Error::OK ? writeError : true;

		// Check for write errors
		if (writeError)
		{
			file.close();
			return NL::Configuration::Error::ERROR_FILE_WRITE;
		}
	}

	// WiFi configuration
	bool writeError = false;
	writeError = file.writeString(NL::Configuration::wifiConfig.accessPointSsid) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.writeString(NL::Configuration::wifiConfig.accessPointPassword) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::wifiConfig.accessPointChannel) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::wifiConfig.accessPointHidden) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::wifiConfig.accessPointMaxConnections) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.writeString(NL::Configuration::wifiConfig.wifiSsid) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.writeString(NL::Configuration::wifiConfig.wifiPassword) == NL::BinaryFile::Error::OK ? writeError : true;

	// Motion sensor calibration
	writeError = file.write(NL::Configuration::motionSensorCalibration.accXRaw) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.accYRaw) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.accZRaw) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.gyroXRaw) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.gyroYRaw) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.gyroZRaw) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.accXG) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.accYG) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.accZG) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.gyroXDeg) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.gyroYDeg) == NL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(NL::Configuration::motionSensorCalibration.gyroZDeg) == NL::BinaryFile::Error::OK ? writeError : true;

	// Audio unit configuration
	writeError = file.write(NL::Configuration::audioUnitConfig.noiseThreshold) == NL::BinaryFile::Error::OK ? writeError : true;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		writeError = file.write(NL::Configuration::audioUnitConfig.frequencyBandIndex[i].first) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(NL::Configuration::audioUnitConfig.frequencyBandIndex[i].second) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence) == NL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(NL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate) == NL::BinaryFile::Error::OK ? writeError : true;
	}

	// Write the hash
	writeError = file.write(NL::Configuration::getSimpleHash()) == NL::BinaryFile::Error::OK ? writeError : true;

	file.close();
	return !writeError ? NL::Configuration::Error::OK : NL::Configuration::Error::ERROR_FILE_WRITE;
}

/**
 * @brief Load the default, profile depending settings for the currently selected profile.
 * @param profileIndex index of the profile to reset
 * @return OK when the profile was reset
 * @return ERROR_OUT_OF_BOUNDS when the profile index is invalid
 */
NL::Configuration::Error NL::Configuration::loadProfileDefaults(const size_t profileIndex)
{
	if (profileIndex >= NL::Configuration::profiles.size())
	{
		return NL::Configuration::Error::ERROR_OUT_OF_BOUNDS;
	}

	// Get the currently active profile
	NL::Configuration::Profile profile = NL::Configuration::profiles.at(profileIndex);

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
	NL::Configuration::profiles.at(profileIndex) = profile;
	return NL::Configuration::Error::OK;
}

/**
 * @brief Get the index of a profile by the profile name.
 * @param profileName name of the profile
 * @param profileIndex reference variable holding the index
 * @return OK when the profile was found
 * @return ERROR_PROFILE_NOT_FOUND when no profile with the given name was found
 */
NL::Configuration::Error NL::Configuration::getProfileIndexByName(const String &profileName, size_t &profileIndex)
{
	for (size_t i = 0; i < NL::Configuration::profiles.size(); i++)
	{
		if (NL::Configuration::profiles.at(i).name == profileName)
		{
			profileIndex = i;
			return NL::Configuration::Error::OK;
		}
	}
	return NL::Configuration::Error::ERROR_PROFILE_NOT_FOUND;
}

/**
 * @brief Calculate a very simple hash to check the config file.
 * @return uint16_t 2 byte hash
 */
uint16_t NL::Configuration::getSimpleHash()
{
	uint16_t hash = 7;

	// Configuration file version
	hash = hash * 31 + NL::Configuration::configurationVersion;

	// Configuration profile count
	hash = hash * 31 + NL::Configuration::profiles.size();

	// Configuration active profile
	hash = hash * 31 + NL::Configuration::activeProfile;

	// Run over all profiles
	for (size_t i = 0; i < NL::Configuration::profiles.size(); i++)
	{
		// Get the profile
		NL::Configuration::Profile profile = NL::Configuration::profiles.at(i);

		// Profile name
		hash = hash * 31 + NL::Configuration::getSimpleStringHash(profile.name);

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
		hash = hash * 32 + NL::Configuration::getSimpleStringHash(profile.uiConfiguration.language);
		hash = hash * 32 + NL::Configuration::getSimpleStringHash(profile.uiConfiguration.theme);
		hash = hash * 32 + profile.uiConfiguration.expertMode;
	}

	// WiFi configuration
	hash = hash * 31 + NL::Configuration::getSimpleStringHash(NL::Configuration::wifiConfig.accessPointSsid);
	hash = hash * 31 + NL::Configuration::getSimpleStringHash(NL::Configuration::wifiConfig.accessPointPassword);
	hash = hash * 31 + NL::Configuration::wifiConfig.accessPointChannel;
	hash = hash * 31 + NL::Configuration::wifiConfig.accessPointHidden;
	hash = hash * 31 + NL::Configuration::wifiConfig.accessPointMaxConnections;
	hash = hash * 31 + NL::Configuration::getSimpleStringHash(NL::Configuration::wifiConfig.wifiSsid);
	hash = hash * 31 + NL::Configuration::getSimpleStringHash(NL::Configuration::wifiConfig.wifiPassword);

	// Motion sensor calibration
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.accXRaw;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.accYRaw;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.accZRaw;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.gyroXRaw;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.gyroYRaw;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.gyroZRaw;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.accXG;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.accYG;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.accZG;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.gyroXDeg;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.gyroYDeg;
	hash = hash * 31 + NL::Configuration::motionSensorCalibration.gyroZDeg;

	// Audio unit configuration
	hash = hash * 31 + NL::Configuration::audioUnitConfig.noiseThreshold;
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		hash = hash * 31 + NL::Configuration::audioUnitConfig.frequencyBandIndex[i].first;
		hash = hash * 31 + NL::Configuration::audioUnitConfig.frequencyBandIndex[i].second;
		hash = hash * 31 + NL::Configuration::audioUnitConfig.peakDetectorConfig[i].historySize;
		hash = hash * 31 + NL::Configuration::audioUnitConfig.peakDetectorConfig[i].threshold;
		hash = hash * 31 + NL::Configuration::audioUnitConfig.peakDetectorConfig[i].influence;
		hash = hash * 31 + NL::Configuration::audioUnitConfig.peakDetectorConfig[i].noiseGate;
	}

	return hash;
}

/**
 * @brief Create a very simple hash based on a string.
 * @param input input string
 * @return uint16_t 2 byte hash
 */
uint16_t NL::Configuration::getSimpleStringHash(const String input)
{
	uint16_t hash = 7;
	for (uint16_t i = 0; i < input.length(); i++)
	{
		hash = hash * 31 + input.charAt(i);
	}
	return hash;
}