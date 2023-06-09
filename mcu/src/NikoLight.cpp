/**
 * @file NikoLight.h
 * @author TheRealKasumi
 * @brief Static class encapsulating all NikoLight functionality.
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
#include "NikoLight.h"

unsigned long NikoLight::lightSensorInterval = LIGHT_SENSOR_INTERVAL;
unsigned long NikoLight::motionSensorInterval = MOTION_SENSOR_INTERVAL;
unsigned long NikoLight::audioUnitInterval = AUDIO_UNIT_INTERVAL;
unsigned long NikoLight::frameTimer = 0;
unsigned long NikoLight::lightSensorTimer = 0;
unsigned long NikoLight::motionSensorTimer = 0;
unsigned long NikoLight::audioUnitTimer = 0;
unsigned long NikoLight::temperatureTimer = 0;
unsigned long NikoLight::statusTimer = 0;
unsigned long NikoLight::statusPrintTimer = 0;
unsigned long NikoLight::webServerTimer = 0;

uint16_t NikoLight::frameCounter = 0;
float NikoLight::ledPowerCounter = 0.0f;

/**
 * @brief Initialize the NikoLight software and hardware.
 */
void NikoLight::begin()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.println(F("Starting up..."));
	NikoLight::printLogo();
	NikoLight::initializeLogger(false);		  // Initialize the logger in serial mode
	NikoLight::initializeSdCard();			  // Initialized the SD card
	NikoLight::initializeLogger(true);		  // Switch to SD card logging
	NikoLight::handleUpdate();				  // Check for system updates and install
	NikoLight::initializeSystemInformation(); // Initialize and print the soc information
	NikoLight::initializeConfiguration();	  // Initialize the configuration
	NikoLight::initializeHardwareModules();	  // Initialize hardware modules and print information
	NikoLight::initializeLedManager();		  // Initialize the LED manager
	NikoLight::initializeMotionSensor();	  // Initalize the motion sensor
	NikoLight::initializeLightSensor();		  // Initialize the light sensor
	NikoLight::initializeTemperatureSensor(); // Initialize the temperature sensor
	NikoLight::initializeFan();				  // Initialize the fan controller
	NikoLight::initializeWiFiManager();		  // Initialize the WiFi manager
	NikoLight::initializeWebServerManager();  // Initialize the web server manager
	NikoLight::initializeRestApi();			  // Iniaialize the rest api
	NikoLight::createtWiFiNetwork();		  // Create the WiFi network for clients to connect to
	NikoLight::initializeTimers();			  // Initialize the timers
	NL::WatchDog::initializeTaskWatchdog();	  // Initialize the watchdog timer
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("NikoLight initialized successfully, going into work mode."));
}

/**
 * @brief Print the NikoLight logo and FW version.
 */
void NikoLight::printLogo()
{
	Serial.println();
	Serial.println((String)F("NikoLight Firmware version ") + String(FW_VERSION));
	Serial.println();
}

/**
 * @brief Initialize the serial and SD card logger. In case the initialization fails, the controller will restart.
 */
void NikoLight::initializeLogger(const bool sdLogging)
{
	if (NL::Logger::isInitialized())
	{
		if (sdLogging)
		{
			NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting logger in serial and file logging mode."));
		}
		else
		{
			NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting logger in serial logging mode."));
		}
	}

	if (!sdLogging)
	{
		NL::Logger::begin(SERIAL_BAUD_RATE, NL::Logger::LogLevel::INFO);
	}
	else
	{
		NL::Logger::begin(SERIAL_BAUD_RATE, &SD, LOG_FILE_NAME, NL::Logger::LogLevel::INFO);
	}

	if (NL::Logger::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Logger was started successfully."));
	}
	else
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Logger could not be started. Rebooting."));
		NL::Updater::reboot(F("Logger failed to start."), 0);
	}
}

/**
 * @brief Initialize the SD card. In case the initialization fails, the controller will restart.
 */
void NikoLight::initializeSdCard()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize MicroSD card."));
	if (SD.begin(SD_CS_PIN, SPI, SD_SPI_SPEED, SD_MOUNT_POINT, SD_MAX_FILES, false))
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("MicroSD card initialized."));
	}
	else
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("MicroSD card could not be initialized. Rebooting."));
		NL::Updater::reboot(F("MicroSD could not be initialized."), 0);
	}
}

/**
 * @brief Initialize the system information and print important information.
 */
void NikoLight::initializeSystemInformation()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize and collect system information."));
	NL::SystemInformation::begin();
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Hardware version: ") + NL::SystemInformation::getHardwareInfo().hwVersion);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Firmware version: ") + NL::SystemInformation::getSocInfo().fwVersion);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Chip model: ") + NL::SystemInformation::getSocInfo().chipModel);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Chip revision: ") + NL::SystemInformation::getSocInfo().chipRevision);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("CPU cores: ") + NL::SystemInformation::getSocInfo().cpuCores);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("CPU clock: ") + NL::SystemInformation::getSocInfo().cpuClock);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Flash size: ") + NL::SystemInformation::getSocInfo().flashSize);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Flash speed: ") + NL::SystemInformation::getSocInfo().flashSpeed);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Sketch size: ") + NL::SystemInformation::getSocInfo().sketchSize);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Sketch free space: ") + NL::SystemInformation::getSocInfo().freeSketchSpace);
}

/**
 * @brief Initialize and load the NikoLight configuraiton.
 * 		  When the configuration can not be loaded, it will continue with defaults.
 */
void NikoLight::initializeConfiguration()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize NikoLight configuration."));
	NL::Configuration::begin(&SD, CONFIGURATION_FILE_NAME);

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Load NikoLight configuration from MicroSD card."));
	const NL::Configuration::Error configError = NL::Configuration::load();
	if (configError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Configuration file could not be found on MicroSD card. Loading defaults."));
		NL::Configuration::loadDefaults();
	}
	else if (configError == NL::Configuration::Error::ERROR_FILE_READ)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Configuration file could not be read from the MicroSD card. Loading defaults."));
		NL::Configuration::loadDefaults();
	}
	else if (configError == NL::Configuration::Error::ERROR_FILE_VERSION)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Configuration file version is outdated. Reset to defaults."));
		NL::Configuration::loadDefaults();
	}
	else if (configError == NL::Configuration::Error::ERROR_FILE_HASH)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Configuration file is corrupted. Loading defaults."));
		NL::Configuration::loadDefaults();
	}
	else if (configError == NL::Configuration::Error::ERROR_TOO_MANY_PROFILES)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The configuration file contains too many profiles. Loading defaults."));
		NL::Configuration::loadDefaults();
	}
	else if (configError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error loading configuration from MicroSD card. Loading defaults."));
		NL::Configuration::loadDefaults();
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("NikoLight configuration initialized."));
}

/**
 * @brief Initialize harware modules on the I²C and OneWire bus.
 * 		  If the initialization fails, the controller will restart.
 * 		  The controller will not restart for missing or failed hardware modules.
 */
void NikoLight::initializeHardwareModules()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize analog input."));
	NL::AnalogInput::begin(ANALOG_INPUT_PIN, ANALOG_INPUT_MODE, ANALOG_INPUT_MAX_VOLTAGE);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Analog input initialized."));

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Initialize I²C bus at ") + IIC_FREQUENCY / 1000 + F("kHz."));
	if (Wire.begin(static_cast<int>(IIC_SDA_PIN), static_cast<int>(IIC_SCL_PIN), static_cast<uint32_t>(IIC_FREQUENCY)))
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("I²C bus initialized."));
	}
	else
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize I²C bus. Rebooting"));
		NL::Updater::reboot(F("Failed to initialize I²C bus."), 0);
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Search and initialize hardware modules."));
	NL::MPU6050::begin(MPU6050_IIC_ADDRESS);
	NL::DS18B20::begin(ONE_WIRE_PIN);
	NL::BH1750::begin(BH1750_IIC_ADDRESS, NL::BH1750::BH1750Res::BH1750_LOW);
	NL::AudioUnit::begin(AUDIO_UNIT_IIC_ADDRESS);

	if (NL::AudioUnit::isInitialized())
	{
		if (NL::AudioUnit::getFrequencyBandCount() == AUDIO_UNIT_NUM_BANDS)
		{
			NL::AudioUnit::AudioUnitConfig audioUnitConfig;
			audioUnitConfig.noiseThreshold = NL::Configuration::getAudioUnitConfig().noiseThreshold;
			audioUnitConfig.frequencyBandIndex.resize(AUDIO_UNIT_NUM_BANDS);
			for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
			{
				audioUnitConfig.frequencyBandIndex.at(i) = NL::Configuration::getAudioUnitConfig().frequencyBandIndex[i];
			}

			const NL::AudioUnit::Error audioConfigError = NL::AudioUnit::setAudioUnitConfig(audioUnitConfig);
			if (audioConfigError != NL::AudioUnit::Error::OK)
			{
				NL::AudioUnit::end();
			}

			for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
			{
				const NL::AudioUnit::Error audioError = NL::AudioUnit::setPeakDetectorConfig(NL::Configuration::getAudioUnitConfig().peakDetectorConfig[i], i);
				if (audioError != NL::AudioUnit::Error::OK)
				{
					NL::AudioUnit::end();
				}
			}
		}
		else
		{
			NL::AudioUnit::end();
		}
	}

	NL::SystemInformation::HardwareInformation hwInfo = NL::SystemInformation::getHardwareInfo();
	hwInfo.mpu6050 = NL::MPU6050::isInitialized();
	hwInfo.ds18b20 = NL::DS18B20::isInitialized() ? NL::DS18B20::getNumSensors() : 0;
	hwInfo.bh1750 = NL::BH1750::isInitialized();
	hwInfo.audioUnit = NL::AudioUnit::isInitialized();
	NL::SystemInformation::setHardwareInfo(hwInfo);

	if (hwInfo.mpu6050 > 0)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Found MPU6050 motion sensor."));
	}
	if (hwInfo.ds18b20 > 0)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Found ") + hwInfo.ds18b20 + F(" DS18B20 temperature sensors."));
	}
	if (hwInfo.bh1750 > 0)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Found BH1750 light sensor."));
	}
	if (hwInfo.audioUnit > 0)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Found NikoLight Audio Unit."));
	}
}

/**
 * @brief Initialize the LED manager. It has a dependency on the {@link NL::Configuration} which must be initialized
 * 		  beforehand. If it was not initialized before calling this function, the controller will reboot.
 */
void NikoLight::initializeLedManager()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize LED manager."));
	const NL::LedManager::Error ledMangerInitError = NL::LedManager::begin();
	if (ledMangerInitError == NL::LedManager::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize LED manager. The configuration must be initialized beforehand. Rebooting."));
		NL::Updater::reboot(F("Failed to initialize LED manager."), 0);
	}
	else if (ledMangerInitError != NL::LedManager::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize LED manager due to unknown error."));
		NL::Updater::reboot(F("Failed to initialize LED manager."), 0);
	}
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED manager initialized. Loading animations."));

	const NL::LedManager::Error ledManagerLoadError = NL::LedManager::reloadAnimations();
	if (ledManagerLoadError == NL::LedManager::Error::ERROR_INIT_LED_DRIVER)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. The LED driver could not be initialized for the LED configuration."));
	}
	else if (ledManagerLoadError == NL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. One of the animator types is unknown. Continuing without LEDs."));
	}
	else if (ledManagerLoadError == NL::LedManager::Error::ERROR_INVALID_FSEQ)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. The selected fseq file is invalid. Continuing without LEDs."));
	}
	else if (ledManagerLoadError == NL::LedManager::Error::ERROR_FILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. No fseq file with selected file id was found. Continuing without LEDs."));
	}
	else if (ledManagerLoadError == NL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. The current configurationn does not match the configuration of the fseq file. Continuing without LEDs."));
	}
	else if (ledManagerLoadError != NL::LedManager::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration because of unknown error. Continuing without LEDs."));
	}
}

/**
 * @brief Initialize the motion sensor. It has a dependency on the {@link NL::Configuration} and {@link NL::MPU6050}.
 * 		  These must be initialized beforehand. If they were not initialized before calling this function, the motion
 * 		  sensor will not be available.
 */
void NikoLight::initializeMotionSensor()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize motion sensor."));
	const NL::MotionSensor::Error motionSensorError = NL::MotionSensor::begin();
	if (motionSensorError == NL::MotionSensor::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Failed to initialize motion sensor because the configuration is not initialized."));
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor will not be available."));
		return;
	}
	else if (motionSensorError == NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No MPU6050 module could be found."));
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor will not be available."));
		return;
	}
	else if (motionSensorError != NL::MotionSensor::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error, motion sensor will not be available."));
		return;
	}
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor initialized."));
}

/**
 * @brief Initialize the light sensor. It has a dependency on the {@link NL::Configuration} which must be initialized
 * 		  beforehand. If it was not initialized before calling this function, the light sensor will not be available.
 */
void NikoLight::initializeLightSensor()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize light sensor."));
	const NL::LightSensor::Error lightSensorError = NL::LightSensor::begin();
	if (lightSensorError == NL::LightSensor::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Failed to initialize light sensor because the configuration is not initialized."));
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Light sensor will not be available."));
		return;
	}
	else if (lightSensorError != NL::LightSensor::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error, light sensor will not be available."));
		return;
	}
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Light sensor initialized."));
}

/**
 * @brief Initialize the temperature sensor. It has a dependency on the {@link NL::DS18B20} which must be initialized
 * 		  beforehand. If it was not initialized before calling this function, the temperature sensor will not be available.
 */
void NikoLight::initializeTemperatureSensor()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize temperature sensor."));
	const NL::TemperatureSensor::Error tempSensorError = NL::TemperatureSensor::begin();
	if (tempSensorError == NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No DS18B20 sensor could be found."));
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Temperature sensor will not be available."));
		return;
	}
	else if (tempSensorError != NL::TemperatureSensor::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error, temperature sensor will not be available."));
		return;
	}
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Temperature sensor initialized. Using ") + NL::DS18B20::getNumSensors() + F(" detected sensors."));
}

/**
 * @brief Initialize the fan controller. It has a dependency on the {@link NL::Configuration} which must be initialized beforehand.
 * 		  If it was not initialized before calling this function, the controller will reboot.
 */
void NikoLight::initializeFan()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize fan controller."));
	const NL::Fan::Error fanError = NL::Fan::begin(FAN_PWM_PIN, FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	if (fanError == NL::Fan::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller because the configuration is not initialized. Rebooting."));
		NL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}
	else if (fanError == NL::Fan::Error::ERROR_TEMP_UNAVAILABLE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller because the temperature sensor is not initialized. Rebooting."));
		NL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}
	else if (fanError == NL::Fan::Error::ERROR_SETUP_PIN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller because the pwm channel could not be configured. Rebooting."));
		NL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}
	else if (fanError != NL::Fan::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller due to unknown error. Rebooting."));
		NL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Fan controller initialized."));
}

/**
 * @brief Initialize the WiFi manager. In case it fails, the controller will reboot.
 */
void NikoLight::initializeWiFiManager()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize WiFi manager."));
	const NL::WiFiManager::Error wifiError = NL::WiFiManager::begin();
	if (wifiError == NL::WiFiManager::Error::ERROR_SET_MODE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set WiFi mode. Rebooting."));
		NL::Updater::reboot(F("Failed to set WiFi mode."), 0);
	}
	else if (wifiError != NL::WiFiManager::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi. Rebooting."));
		NL::Updater::reboot(F("Failed to start WiFi."), 0);
	}
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi manager initialized."));
}

/**
 * @brief Initialize the web server manager.
 */
void NikoLight::initializeWebServerManager()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize webserver."));
	NL::WebServerManager::begin(&SD, WEB_SERVER_PORT);
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Webserver initialized."));
}

/**
 * @brief Initialize the rest api.
 */
void NikoLight::initializeRestApi()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize REST API."));
	NL::ConnectionTestEndpoint::init(F("/api/"));
	NL::ConnectionTestEndpoint::begin();
	NL::ProfileEndpoint::init(F("/api/"));
	NL::ProfileEndpoint::begin();
	NL::SystemInformationEndpoint::init(F("/api/"));
	NL::SystemInformationEndpoint::begin();
	NL::SystemConfigurationEndpoint::init(F("/api/"));
	NL::SystemConfigurationEndpoint::begin();
	NL::LedConfigurationEndpoint::init(F("/api/"));
	NL::LedConfigurationEndpoint::begin();
	NL::WiFiConfigurationEndpoint::init(F("/api/"));
	NL::WiFiConfigurationEndpoint::begin();
	NL::FseqEndpoint::init(F("/api/"));
	NL::FseqEndpoint::begin(&SD);
	NL::LogEndpoint::init(F("/api/"));
	NL::LogEndpoint::begin(&SD);
	NL::UpdateEndpoint::init(F("/api/"));
	NL::UpdateEndpoint::begin(&SD);
	NL::ResetEndpoint::init(F("/api/"));
	NL::ResetEndpoint::begin(&SD);
	NL::MotionSensorEndpoint::init(F("/api/"));
	NL::MotionSensorEndpoint::begin();
	NL::AudioUnitConfigurationEndpoint::init(F("/api/"));
	NL::AudioUnitConfigurationEndpoint::begin();
	NL::UIConfigurationEndpoint::init(F("/api/"));
	NL::UIConfigurationEndpoint::begin();
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("REST API initialized."));

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Starting web server on port ") + WEB_SERVER_PORT + F("."));
	NL::WebServerManager::startServer();
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Web server started."));
}

/**
 * @brief Initialize or reset timers.
 */
void NikoLight::initializeTimers()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize/reset timers."));
	unsigned long mic = micros();
	NikoLight::frameTimer = mic;
	NikoLight::lightSensorTimer = mic;
	NikoLight::motionSensorTimer = mic;
	NikoLight::audioUnitTimer = mic;
	NikoLight::temperatureTimer = mic;
	NikoLight::statusTimer = mic;
	NikoLight::statusPrintTimer = mic;
	NikoLight::webServerTimer = mic;
	NikoLight::frameCounter = 0;
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Timers initialized to ") + mic + F("."));
}

/**
 * @brief Check if a system update is available for installation. In case a update package was found,
 * 		  it will be installed automatically. This function will restart the controller and will not
 *		  return in case that a TUP was found and the update was installed.
 */
void NikoLight::handleUpdate()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Check if NikoLight update package is available for installation."));
	if (!NL::FileUtil::fileExists(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME))
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No update package found."));
		return;
	}
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("NikoLight update package found. Installing system update."));

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Set fan speed to maximum during the update."));
	pinMode(FAN_PWM_PIN, OUTPUT);
	digitalWrite(FAN_PWM_PIN, HIGH);

	const NL::Updater::Error updateError = NL::Updater::install(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME);
	if (updateError == NL::Updater::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System update installed successfully. Rebooting."));
		NL::Updater::reboot(F("System update reboot"), 0);
	}
	else if (updateError == NL::Updater::Error::ERROR_UPDATE_FILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The update package could not be found."));
	}
	else if (updateError == NL::Updater::Error::ERROR_INVALID_FILE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The update package is invalid."));
	}
	else if (updateError == NL::Updater::Error::ERROR_CLEAN_FS)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The file system root could not be cleared. The MicroSD card might be corrupted. Trying to continue boot."));
	}
	else if (updateError == NL::Updater::Error::ERROR_UPDATE_UNPACK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The update package could not be unpacked. The MicroSD card might be corrupted. Trying to continue boot."));
	}
	else if (updateError == NL::Updater::Error::ERROR_FW_FILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The firmware file could not be found. Trying to continue boot."));
	}
	else if (updateError == NL::Updater::Error::ERROR_FW_FILE_EMPTY)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The firmware file is empty. Trying to continue boot."));
	}
	else if (updateError == NL::Updater::Error::ERROR_OUT_OF_FLASH_MEMORY)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. Not enough flash memory for firmware update. Rebooting."));
		NL::Updater::reboot(F("Not enough flash memory for firmware update."), 0);
	}
	else if (updateError == NL::Updater::Error::ERROR_WRITE_FW_DATA)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. Could not write firmware data to flash memory. It might be required to reinstall the firmware via the USB port. Rebooting."));
		NL::Updater::reboot(F("Could not write firmware data to flash memory."), 0);
	}
	else if (updateError == NL::Updater::Error::ERROR_FINISH_FW_UPDATE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. Could not finish firmware update. It might be required to reinstall the firmware via the USB port. Rebooting."));
		NL::Updater::reboot(F("Could not finish firmware update."), 0);
	}
	else
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed due to an unknown error. It might be required to reinstall the firmware via the USB port. Rebooting."));
		NL::Updater::reboot(F("System update failed."), 0);
	}
}

/**
 * @brief Create a WiFi access point for clients to connect to. In case the creation fails,
 * 		  the default configuration will be used to try to restore access to the controller.
 */
void NikoLight::createtWiFiNetwork()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Creating WiFi network."));
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting WiFi access point."));
	uint8_t retry = 0;
	do
	{
		const NL::WiFiManager::Error wifiError = NL::WiFiManager::startAccessPoint(NL::Configuration::getWiFiConfig().accessPointSsid.c_str(), NL::Configuration::getWiFiConfig().accessPointPassword.c_str(), NL::Configuration::getWiFiConfig().accessPointChannel, false, NL::Configuration::getWiFiConfig().accessPointMaxConnections);
		if (wifiError == NL::WiFiManager::Error::OK)
		{
			NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi network created."));
			return;
		}
		else if (wifiError == NL::WiFiManager::Error::ERROR_START_AP)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
		}
		else if (wifiError == NL::WiFiManager::Error::ERROR_INVALID_SSID)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The SSID is invalid."));
		}
		else if (wifiError == NL::WiFiManager::Error::ERROR_INVALID_PW)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The password is invalid."));
		}
		else
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
		}

		if (retry == 0)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. Loading default settings and retry."));
			NL::Configuration::WiFiConfig wifiConfig = NL::Configuration::getWiFiConfig();
			wifiConfig.accessPointSsid = F(AP_DEFAULT_SSID);
			wifiConfig.accessPointPassword = F(AP_DEDAULT_PASSWORD);
			wifiConfig.accessPointChannel = AP_DEFAULT_CHANNEL;
			wifiConfig.accessPointHidden = AP_DEFAULT_HIDDEN;
			wifiConfig.accessPointMaxConnections = AP_DEFAULT_MAX_CONN;
			wifiConfig.wifiSsid = F(WIFI_DEFAULT_SSID);
			wifiConfig.wifiPassword = F(WIFI_DEFAULT_PASSWORD);
			NL::Configuration::setWiFiConfig(wifiConfig);
		}
		else
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
		}

	} while (retry++ < 1);
}

/**
 * @brief Check if a timer expired.
 * @param timer timer value
 * @param cycleTime cycle time of the timer
 * @return true when the timer expired
 * @return false when the timer is not expired yet
 */
bool NikoLight::checkTimer(unsigned long &timer, unsigned long cycleTime)
{
	unsigned long mic = micros();
	if (mic - timer > cycleTime)
	{
		timer += cycleTime;
		return true;
	}
	return false;
}

/**
 * @brief Function should be called continuously from the main loop.
 */
void NikoLight::run()
{
	// Handle the pixel rendering and LED output
	if (NikoLight::checkTimer(NikoLight::frameTimer, NL::LedManager::getFrameInterval()))
	{
		NL::LedManager::render();
		NL::LedManager::show(portMAX_DELAY);
		NL::LedManager::waitShow(portMAX_DELAY);
		NikoLight::frameCounter++;
		NikoLight::ledPowerCounter += NL::LedManager::getLedPowerDraw();
	}

	// Handle the light sensor
	if (NikoLight::checkTimer(NikoLight::lightSensorTimer, NikoLight::lightSensorInterval) && NL::LightSensor::isInitialized())
	{
		float brightness;
		const NL::LightSensor::Error lightSensorError = NL::LightSensor::getBrightness(brightness);
		if (lightSensorError == NL::LightSensor::Error::OK)
		{
			NL::LedManager::setAmbientBrightness(brightness);
			NikoLight::lightSensorInterval = LIGHT_SENSOR_INTERVAL;
		}
		else
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read light sensor data. Delaying next read by 1s."));
			NL::LedManager::setAmbientBrightness(1.0f);
			NikoLight::lightSensorInterval = 1000000;
		}
	}

	// Handle the motion sensor
	if (NikoLight::checkTimer(NikoLight::motionSensorTimer, NikoLight::motionSensorInterval) && NL::MotionSensor::isInitialized())
	{
		const NL::MotionSensor::Error motionSensorError = NL::MotionSensor::run();
		if (motionSensorError == NL::MotionSensor::Error::OK)
		{
			NL::LedManager::setMotionSensorData(NL::MotionSensor::getMotion());
			NikoLight::motionSensorInterval = MOTION_SENSOR_INTERVAL;
		}
		else
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read motion sensor data. Delaying next read by 1s"));
			NikoLight::motionSensorInterval = 1000000;
		}
	}

	// Handle the audio unit
	if (NikoLight::checkTimer(NikoLight::audioUnitTimer, NikoLight::audioUnitInterval) && NL::AudioUnit::isInitialized())
	{
		NL::AudioUnit::AudioAnalysis audioAnalysis;
		const NL::AudioUnit::Error audioError = NL::AudioUnit::getAudioAnalysis(audioAnalysis);
		if (audioError == NL::AudioUnit::Error::OK)
		{
			NL::LedManager::setAudioAnalysis(audioAnalysis);
			NikoLight::audioUnitInterval = AUDIO_UNIT_INTERVAL;
		}
		else
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read audio analysis data. Delaying next read by 1s"));
			NikoLight::audioUnitInterval = 1000000;
		}
	}

	// Handle the fan controller
	if (NikoLight::checkTimer(NikoLight::temperatureTimer, FAN_INTERVAL))
	{
		const NL::Fan::Error fanError = NL::Fan::run(static_cast<NL::Fan::FanMode>(NL::Configuration::getSystemConfig().fanMode));
		if (fanError == NL::Fan::Error::ERROR_TEMP_UNAVAILABLE)
		{
			NL::Logger::log(NL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Failed to update cooling fan because the temperature could not be read. Using fallback."));
		}
		else if (fanError != NL::Fan::Error::OK)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to update cooling fan."));
		}

		NL::SystemInformation::HardwareInformation hwInfo = NL::SystemInformation::getHardwareInfo();
		hwInfo.fanSpeed = NL::Fan::getPwmValue();
		NL::SystemInformation::setHardwareInfo(hwInfo);
	}

	// Update the soc, led and hardware information
	if (NikoLight::checkTimer(NikoLight::statusTimer, STATUS_INTERVAL))
	{
		// Update the SOC information
		NL::SystemInformation::updateSocInfo();

		// Update LED related information
		NL::SystemInformation::NLInformation tlInfo = NL::SystemInformation::getNikoLightInfo();
		tlInfo.fps = frameCounter / (STATUS_INTERVAL / 1000000.0f);
		tlInfo.ledCount = NL::LedManager::getLedCount();
		tlInfo.hiddenLedCount = NL::LedManager::getHiddenLedCount();
		NL::SystemInformation::setNikoLightInfo(tlInfo);

		// Update regulator related information
		NL::SystemInformation::HardwareInformation hwInfo = NL::SystemInformation::getHardwareInfo();
		hwInfo.regulatorPowerDraw = ledPowerCounter / frameCounter;
		hwInfo.regulatorCurrentDraw = hwInfo.regulatorPowerDraw / hwInfo.regulatorVoltage;
		if (NL::TemperatureSensor::isInitialized())
		{
			if (NL::TemperatureSensor::getMaxTemperature(hwInfo.regulatorTemperature) != NL::TemperatureSensor::Error::OK)
			{
				hwInfo.regulatorTemperature = 0.0f;
			}
		}
		else
		{
			hwInfo.regulatorTemperature = 0.0f;
		}
		NL::SystemInformation::setHardwareInfo(hwInfo);

		NikoLight::frameCounter = 0;
		NikoLight::ledPowerCounter = 0.0f;
	}

	// Print the system status
	if (NikoLight::checkTimer(NikoLight::statusPrintTimer, STATUS_PRINT_INTERVAL))
	{
		const NL::SystemInformation::SocInfo socInfo = NL::SystemInformation::getSocInfo();
		const NL::SystemInformation::NLInformation tlInfo = NL::SystemInformation::getNikoLightInfo();
		const NL::SystemInformation::HardwareInformation hwInfo = NL::SystemInformation::getHardwareInfo();
		NL::Logger::log(
			NL::Logger::LogLevel::INFO,
			SOURCE_LOCATION,
			(String)F("LED Driver: ") + tlInfo.fps + F("FPS   ") +
				F("Average Power: ") + hwInfo.regulatorPowerDraw + F("W   ") +
				F("Average Current: ") + hwInfo.regulatorCurrentDraw + F("A   ") +
				F("Temperature: ") + hwInfo.regulatorTemperature + F("°C   ") +
				F("Fan: ") + hwInfo.fanSpeed / 2.55f + F("%   ") +
				F("Heap (free): ") + socInfo.freeHeap + F("Bytes"));
	}

	// Handle web server requests
	if (NikoLight::checkTimer(NikoLight::webServerTimer, WEB_SERVER_INTERVAL))
	{
		// Stop the watchdog to not trigger it during longer requests
		NL::WatchDog::deleteTaskWatchdog();

		// Run the server and measure execution time
		unsigned long start = millis();
		NL::WebServerManager::handleRequest();
		unsigned long executionTime = millis() - start;
		if (executionTime > 2000)
		{
			// When the execution time was longer than 2s, reset the timers
			initializeTimers();
		}

		// Start the watchdog again
		NL::WatchDog::initializeTaskWatchdog();
	}

	// Reset the task watchdog timer
	NL::WatchDog::resetTaskWatchdog();
}
