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
TL::Configuration::SystemConfig TL::Configuration::systemConfig;
TL::Configuration::LedConfig TL::Configuration::ledConfig[LED_NUM_ZONES];
TL::Configuration::WiFiConfig TL::Configuration::wifiConfig;
TL::Configuration::MotionSensorCalibration TL::Configuration::motionSensorCalibration;
TL::Configuration::AudioUnitConfig TL::Configuration::audioUnitConfig;
TL::Configuration::UIConfiguration TL::Configuration::uiConfiguration;

/**
 * @brief Initialize the configuration.
 * @param fileSystem pointer to a {@link FS}
 * @param fileName file name of the configuration file
 */
void TL::Configuration::begin(FS *fileSystem, const String fileName)
{
	TL::Configuration::initialized = true;
	TL::Configuration::fileSystem = fileSystem;
	TL::Configuration::fileName = fileName;
	TL::Configuration::configurationVersion = CONFIGURATION_FILE_VERSION;
	TL::Configuration::loadDefaults();
}

/**
 * @brief Deinitialize the configuration.
 */
void TL::Configuration::end()
{
	TL::Configuration::initialized = false;
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
 * @brief Get the system configuration.
 * @return system configuration
 */
TL::Configuration::SystemConfig TL::Configuration::getSystemConfig()
{
	return TL::Configuration::systemConfig;
}

/**
 * @brief Set the system configuration.
 * @param systemConfig new system configuration
 */
void TL::Configuration::setSystemConfig(TL::Configuration::SystemConfig &systemConfig)
{
	TL::Configuration::systemConfig = systemConfig;
}

/**
 * @brief Get the LED configuration for a specific zone.
 * @param index index of the zone
 * @return LED configuration for the zone
 */
TL::Configuration::LedConfig TL::Configuration::getLedConfig(const uint8_t index)
{
	return TL::Configuration::ledConfig[index];
}

/**
 * @brief Set the LED configuration for a specific zone.
 * @param ledConfig new LED configuration for the zone
 * @param index index of the zone
 */
void TL::Configuration::setLedConfig(const TL::Configuration::LedConfig &ledConfig, const uint8_t index)
{
	TL::Configuration::ledConfig[index] = ledConfig;
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
	return TL::Configuration::uiConfiguration;
}

/**
 * @brief Set the UI configuration.
 * @param uiConfiguration new UI configuration
 */
void TL::Configuration::setUIConfiguration(const TL::Configuration::UIConfiguration &uiConfiguration)
{
	TL::Configuration::uiConfiguration = uiConfiguration;
}

/**
 * @brief Load the default configuration.
 */
void TL::Configuration::loadDefaults()
{
	// System config
	TL::Configuration::systemConfig.logLevel = LOG_DEFAULT_LEVEL;
	TL::Configuration::systemConfig.lightSensorMode = LIGHT_SENSOR_DEFAULT_MODE;
	TL::Configuration::systemConfig.lightSensorThreshold = LIGHT_SENSOR_DEFAULT_THRESHOLD;
	TL::Configuration::systemConfig.lightSensorMinAmbientBrightness = LIGHT_SENSOR_DEFAULT_MIN_AMBIENT;
	TL::Configuration::systemConfig.lightSensorMaxAmbientBrightness = LIGHT_SENSOR_DEFAULT_MAX_AMBIENT;
	TL::Configuration::systemConfig.lightSensorMinLedBrightness = LIGHT_SENSOR_DEFAULT_MIN_LED;
	TL::Configuration::systemConfig.lightSensorMaxLedBrightness = LIGHT_SENSOR_DEFAULT_MAX_LED;
	TL::Configuration::systemConfig.lightSensorDuration = LIGHT_SENSOR_DEFAULT_DURATION;
	TL::Configuration::systemConfig.regulatorPowerLimit = REGULATOR_POWER_LIMIT * REGULATOR_COUNT;
	TL::Configuration::systemConfig.regulatorHighTemperature = REGULATOR_HIGH_TEMP;
	TL::Configuration::systemConfig.regulatorCutoffTemperature = REGULATOR_CUT_OFF_TEMP;
	TL::Configuration::systemConfig.fanMode = FAN_DEFAULT_MODE;
	TL::Configuration::systemConfig.fanMinPwmValue = FAN_DEFAULT_PWM_MIN;
	TL::Configuration::systemConfig.fanMaxPwmValue = FAN_DEFAULT_PWM_MAX;
	TL::Configuration::systemConfig.fanMinTemperature = FAN_DEFAULT_TEMP_MIN;
	TL::Configuration::systemConfig.fanMaxTemperature = FAN_DEFAULT_TEMP_MAX;

	// LED config
	const uint8_t ledPins[LED_NUM_ZONES] = LED_DEFAULT_OUTPUT_PINS;
	const uint8_t ledCounts[LED_NUM_ZONES] = LED_DEFAULT_COUNTS;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		TL::Configuration::ledConfig[i].ledPin = ledPins[i];
		TL::Configuration::ledConfig[i].ledCount = ledCounts[i];
		TL::Configuration::ledConfig[i].type = ANIMATOR_DEFAULT_TYPE;
		TL::Configuration::ledConfig[i].speed = ANIMATOR_DEFAULT_SPEED;
		TL::Configuration::ledConfig[i].offset = ANIMATOR_DEFAULT_OFFSET;
		TL::Configuration::ledConfig[i].brightness = ANIMATOR_DEFAULT_BRIGHTNESS;
		TL::Configuration::ledConfig[i].reverse = ANIMATOR_DEFAULT_REVERSE;
		TL::Configuration::ledConfig[i].fadeSpeed = ANIMATOR_DEFAULT_FADE_SPEED;
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			TL::Configuration::ledConfig[i].animationSettings[j] = 0;
		}
		TL::Configuration::ledConfig[i].ledVoltage = REGULATOR_DEFAULT_VOLTAGE;
		TL::Configuration::ledConfig[i].ledChannelCurrent[0] = LED_DEFAULT_CHANNEL_CURRENT;
		TL::Configuration::ledConfig[i].ledChannelCurrent[1] = LED_DEFAULT_CHANNEL_CURRENT;
		TL::Configuration::ledConfig[i].ledChannelCurrent[2] = LED_DEFAULT_CHANNEL_CURRENT;
	}

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

	// UI configuration
	TL::Configuration::uiConfiguration.firmware = FW_VERSION;
	TL::Configuration::uiConfiguration.language = UI_DEFAULT_LANGUAGE;
	TL::Configuration::uiConfiguration.theme = UI_DEFAULT_THEME;
	TL::Configuration::uiConfiguration.expertMode = UI_DEFAULT_EXPERT;
}

/**
 * @brief Load the configuration from a binary file.
 * @param loadDefaultIfNotExist load default values when the file could not be found
 * @return OK when the file was loaded
 * @return ERROR_FILE_OPEN when the file could not be opened
 * @return ERROR_FILE_READ when the file could not be read
 * @return ERROR_FILE_VERSION when the file version does not match
 * @return ERROR_FILE_HASH when the file hash doesn't match
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

	// System config
	bool readError = false;
	readError = file.read(TL::Configuration::systemConfig.logLevel) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.lightSensorMode) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.lightSensorThreshold) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.lightSensorMinAmbientBrightness) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.lightSensorMaxAmbientBrightness) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.lightSensorMinLedBrightness) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.lightSensorMaxLedBrightness) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.lightSensorDuration) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.regulatorPowerLimit) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.regulatorHighTemperature) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.regulatorCutoffTemperature) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.fanMode) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.fanMinPwmValue) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.fanMaxPwmValue) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.fanMinTemperature) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::systemConfig.fanMaxTemperature) == TL::BinaryFile::Error::OK ? readError : true;

	// LED config
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		readError = file.read(TL::Configuration::ledConfig[i].ledPin) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].ledCount) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].type) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].speed) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].offset) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].brightness) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].reverse) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].fadeSpeed) == TL::BinaryFile::Error::OK ? readError : true;
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			readError = file.read(TL::Configuration::ledConfig[i].animationSettings[j]) == TL::BinaryFile::Error::OK ? readError : true;
		}
		readError = file.read(TL::Configuration::ledConfig[i].ledVoltage) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].ledChannelCurrent[0]) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].ledChannelCurrent[1]) == TL::BinaryFile::Error::OK ? readError : true;
		readError = file.read(TL::Configuration::ledConfig[i].ledChannelCurrent[2]) == TL::BinaryFile::Error::OK ? readError : true;
	}

	// WiFi config
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

	// UI configuration
	readError = file.readString(TL::Configuration::uiConfiguration.language) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.readString(TL::Configuration::uiConfiguration.theme) == TL::BinaryFile::Error::OK ? readError : true;
	readError = file.read(TL::Configuration::uiConfiguration.expertMode) == TL::BinaryFile::Error::OK ? readError : true;

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
	bool writeError = false;
	writeError = file.write(TL::Configuration::configurationVersion) == TL::BinaryFile::Error::OK ? writeError : true;

	// System cofiguration
	writeError = file.write(static_cast<uint8_t>(TL::Configuration::systemConfig.logLevel)) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(static_cast<uint8_t>(TL::Configuration::systemConfig.lightSensorMode)) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.lightSensorThreshold) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.lightSensorMinAmbientBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.lightSensorMaxAmbientBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.lightSensorMinLedBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.lightSensorMaxLedBrightness) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.lightSensorDuration) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.regulatorPowerLimit) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.regulatorHighTemperature) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.regulatorCutoffTemperature) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.fanMode) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.fanMinPwmValue) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.fanMaxPwmValue) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.fanMinTemperature) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::systemConfig.fanMaxTemperature) == TL::BinaryFile::Error::OK ? writeError : true;

	// LED configuration
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		writeError = file.write(TL::Configuration::ledConfig[i].ledPin) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].ledCount) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].type) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].speed) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].offset) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].brightness) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].reverse) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].fadeSpeed) == TL::BinaryFile::Error::OK ? writeError : true;
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			writeError = file.write(TL::Configuration::ledConfig[i].animationSettings[j]) == TL::BinaryFile::Error::OK ? writeError : true;
		}
		writeError = file.write(TL::Configuration::ledConfig[i].ledVoltage) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].ledChannelCurrent[0]) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].ledChannelCurrent[1]) == TL::BinaryFile::Error::OK ? writeError : true;
		writeError = file.write(TL::Configuration::ledConfig[i].ledChannelCurrent[2]) == TL::BinaryFile::Error::OK ? writeError : true;
	}

	// WiFi configuration
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

	// UI configuration
	writeError = file.writeString(TL::Configuration::uiConfiguration.language) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.writeString(TL::Configuration::uiConfiguration.theme) == TL::BinaryFile::Error::OK ? writeError : true;
	writeError = file.write(TL::Configuration::uiConfiguration.expertMode) == TL::BinaryFile::Error::OK ? writeError : true;

	// Write the hash
	writeError = file.write(TL::Configuration::getSimpleHash()) == TL::BinaryFile::Error::OK ? writeError : true;

	file.close();
	return !writeError ? TL::Configuration::Error::OK : TL::Configuration::Error::ERROR_FILE_WRITE;
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

	// System configuration
	hash = hash * 31 + TL::Configuration::systemConfig.logLevel;
	hash = hash * 31 + TL::Configuration::systemConfig.lightSensorMode;
	hash = hash * 31 + TL::Configuration::systemConfig.lightSensorThreshold;
	hash = hash * 31 + TL::Configuration::systemConfig.lightSensorMinAmbientBrightness;
	hash = hash * 31 + TL::Configuration::systemConfig.lightSensorMaxAmbientBrightness;
	hash = hash * 31 + TL::Configuration::systemConfig.lightSensorMinLedBrightness;
	hash = hash * 31 + TL::Configuration::systemConfig.lightSensorMaxLedBrightness;
	hash = hash * 31 + TL::Configuration::systemConfig.regulatorPowerLimit;
	hash = hash * 31 + TL::Configuration::systemConfig.regulatorHighTemperature;
	hash = hash * 31 + TL::Configuration::systemConfig.regulatorCutoffTemperature;
	hash = hash * 31 + TL::Configuration::systemConfig.fanMode;
	hash = hash * 31 + TL::Configuration::systemConfig.fanMinPwmValue;
	hash = hash * 31 + TL::Configuration::systemConfig.fanMaxPwmValue;
	hash = hash * 31 + TL::Configuration::systemConfig.fanMinTemperature;
	hash = hash * 31 + TL::Configuration::systemConfig.fanMaxTemperature;

	// LED configuration
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		hash = hash * 31 + TL::Configuration::ledConfig[i].ledPin;
		hash = hash * 31 + TL::Configuration::ledConfig[i].ledCount;
		hash = hash * 31 + TL::Configuration::ledConfig[i].type;
		hash = hash * 31 + TL::Configuration::ledConfig[i].speed;
		hash = hash * 31 + TL::Configuration::ledConfig[i].offset;
		hash = hash * 31 + TL::Configuration::ledConfig[i].brightness;
		hash = hash * 31 + TL::Configuration::ledConfig[i].reverse;
		hash = hash * 31 + TL::Configuration::ledConfig[i].fadeSpeed;
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			hash = hash * 31 + TL::Configuration::ledConfig[i].animationSettings[j];
		}
		hash = hash * 31 + TL::Configuration::ledConfig[i].ledVoltage;
		hash = hash * 31 + TL::Configuration::ledConfig[i].ledChannelCurrent[0];
		hash = hash * 31 + TL::Configuration::ledConfig[i].ledChannelCurrent[1];
		hash = hash * 31 + TL::Configuration::ledConfig[i].ledChannelCurrent[2];
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

	// UI configuration
	hash = hash * 32 + TL::Configuration::getSimpleStringHash(TL::Configuration::uiConfiguration.language);
	hash = hash * 32 + TL::Configuration::getSimpleStringHash(TL::Configuration::uiConfiguration.theme);
	hash = hash * 32 + TL::Configuration::uiConfiguration.expertMode;

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