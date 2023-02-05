/**
 * @file TesLight.h
 * @author TheRealKasumi
 * @brief Static class encapsulating all TesLight functionality.
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
#include "TesLight.h"

unsigned long TesLight::lightSensorInterval = LIGHT_SENSOR_INTERVAL;
unsigned long TesLight::motionSensorInterval = MOTION_SENSOR_INTERVAL;
unsigned long TesLight::audioUnitInterval = AUDIO_UNIT_INTERVAL;
unsigned long TesLight::frameTimer = 0;
unsigned long TesLight::lightSensorTimer = 0;
unsigned long TesLight::motionSensorTimer = 0;
unsigned long TesLight::audioUnitTimer = 0;
unsigned long TesLight::temperatureTimer = 0;
unsigned long TesLight::statusTimer = 0;
unsigned long TesLight::statusPrintTimer = 0;
unsigned long TesLight::webServerTimer = 0;

uint16_t TesLight::frameCounter = 0;
float TesLight::ledPowerCounter = 0.0f;

/**
 * @brief Initialize the TesLight software and hardware.
 */
void TesLight::begin()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.println(F("Starting up..."));
	TesLight::printLogo();
	TesLight::initializeLogger(false);		 // Initialize the logger in serial mode
	TesLight::initializeSdCard();			 // Initialized the SD card
	TesLight::initializeLogger(true);		 // Switch to SD card logging
	TesLight::handleUpdate();				 // Check for system updates and install
	TesLight::initializeSystemInformation(); // Initialize and print the soc information
	TesLight::initializeConfiguration();	 // Initialize the configuration
	TesLight::initializeHardwareModules();	 // Initialize hardware modules and print information
	TesLight::initializeLedManager();		 // Initialize the LED manager
	TesLight::initializeMotionSensor();		 // Initalize the motion sensor
	TesLight::initializeLightSensor();		 // Initialize the light sensor
	TesLight::initializeTemperatureSensor(); // Initialize the temperature sensor
	TesLight::initializeFan();				 // Initialize the fan controller
	TesLight::initializeWiFiManager();		 // Initialize the WiFi manager
	TesLight::initializeWebServerManager();	 // Initialize the web server manager
	TesLight::initializeRestApi();			 // Iniaialize the rest api
	TesLight::createtWiFiNetwork();			 // Create the WiFi network for clients to connect to
	TesLight::initializeTimers();			 // Initialize the timers
	TL::WatchDog::initializeTaskWatchdog();	 // Initialize the watchdog timer
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight initialized successfully, going into work mode."));
}

/**
 * @brief Print the TesLight logo and FW version.
 */
void TesLight::printLogo()
{
	Serial.println();
	Serial.println(F("████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗"));
	Serial.println(F("╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝"));
	Serial.println(F("   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║   "));
	Serial.println(F("   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║   "));
	Serial.println(F("   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║   "));
	Serial.println(F("   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   "));
	Serial.println((String)F("Firmware version ") + String(FW_VERSION));
	Serial.println();
}

/**
 * @brief Initialize the serial and SD card logger. In case the initialization fails, the controller will restart.
 */
void TesLight::initializeLogger(const bool sdLogging)
{
	if (TL::Logger::isInitialized())
	{
		if (sdLogging)
		{
			TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting logger in serial and file logging mode."));
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting logger in serial logging mode."));
		}
	}

	if (!sdLogging)
	{
		TL::Logger::begin(SERIAL_BAUD_RATE, TL::Logger::LogLevel::INFO);
	}
	else
	{
		TL::Logger::begin(SERIAL_BAUD_RATE, &SD, LOG_FILE_NAME, TL::Logger::LogLevel::INFO);
	}

	if (TL::Logger::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Logger was started successfully."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Logger could not be started. Rebooting."));
		TL::Updater::reboot(F("Logger failed to start."), 0);
	}
}

/**
 * @brief Initialize the SD card. In case the initialization fails, the controller will restart.
 */
void TesLight::initializeSdCard()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize MicroSD card."));
	if (SD.begin(SD_CS_PIN, SPI, SD_SPI_SPEED, SD_MOUNT_POINT, SD_MAX_FILES, false))
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("MicroSD card initialized."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("MicroSD card could not be initialized. Rebooting."));
		TL::Updater::reboot(F("MicroSD could not be initialized."), 0);
	}
}

/**
 * @brief Initialize the system information and print important information.
 */
void TesLight::initializeSystemInformation()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize and collect system information."));
	TL::SystemInformation::begin();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Hardware version: ") + TL::SystemInformation::getHardwareInfo().hwVersion);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Firmware version: ") + TL::SystemInformation::getSocInfo().fwVersion);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Chip model: ") + TL::SystemInformation::getSocInfo().chipModel);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Chip revision: ") + TL::SystemInformation::getSocInfo().chipRevision);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("CPU cores: ") + TL::SystemInformation::getSocInfo().cpuCores);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("CPU clock: ") + TL::SystemInformation::getSocInfo().cpuClock);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Flash size: ") + TL::SystemInformation::getSocInfo().flashSize);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Flash speed: ") + TL::SystemInformation::getSocInfo().flashSpeed);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Sketch size: ") + TL::SystemInformation::getSocInfo().sketchSize);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Sketch free space: ") + TL::SystemInformation::getSocInfo().freeSketchSpace);
}

/**
 * @brief Initialize and load the TesLight configuraiton.
 * 		  When the configuration can not be loaded, it will continue with defaults.
 */
void TesLight::initializeConfiguration()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize TesLight configuration."));
	TL::Configuration::begin(&SD, CONFIGURATION_FILE_NAME);

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Load TesLight configuration from MicroSD card."));
	const TL::Configuration::Error configError = TL::Configuration::load();
	if (configError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Configuration file could not be found on MicroSD card. Loading defaults."));
		TL::Configuration::loadDefaults();
	}
	else if (configError == TL::Configuration::Error::ERROR_FILE_READ)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Configuration file could not be read from the MicroSD card. Loading defaults."));
		TL::Configuration::loadDefaults();
	}
	else if (configError == TL::Configuration::Error::ERROR_FILE_VERSION)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Configuration file version is outdated. Reset to defaults."));
		TL::Configuration::loadDefaults();
	}
	else if (configError == TL::Configuration::Error::ERROR_FILE_HASH)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Configuration file is corrupted. Loading defaults."));
		TL::Configuration::loadDefaults();
	}
	else if (configError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error loading configuration from MicroSD card. Loading defaults."));
		TL::Configuration::loadDefaults();
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight configuration initialized."));
}

/**
 * @brief Initialize harware modules on the I²C and OneWire bus.
 * 		  If the initialization fails, the controller will restart.
 * 		  The controller will not restart for missing or failed hardware modules.
 */
void TesLight::initializeHardwareModules()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize analog input."));
	TL::AnalogInput::begin(ANALOG_INPUT_PIN, ANALOG_INPUT_MODE, ANALOG_INPUT_MAX_VOLTAGE);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Analog input initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Initialize I²C bus at ") + IIC_FREQUENCY / 1000 + F("kHz."));
	if (Wire.begin(static_cast<int>(IIC_SDA_PIN), static_cast<int>(IIC_SCL_PIN), static_cast<uint32_t>(IIC_FREQUENCY)))
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("I²C bus initialized."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize I²C bus. Rebooting"));
		TL::Updater::reboot(F("Failed to initialize I²C bus."), 0);
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Search and initialize hardware modules."));
	TL::MPU6050::begin(MPU6050_IIC_ADDRESS);
	TL::DS18B20::begin(ONE_WIRE_PIN);
	TL::BH1750::begin(BH1750_IIC_ADDRESS);
	TL::AudioUnit::begin(AUDIO_UNIT_IIC_ADDRESS);

	if (TL::AudioUnit::isInitialized())
	{
		if (TL::AudioUnit::getFrequencyBandCount() == AUDIO_UNIT_NUM_BANDS)
		{
			TL::AudioUnit::AudioUnitConfig audioUnitConfig;
			audioUnitConfig.noiseThreshold = TL::Configuration::getAudioUnitConfig().noiseThreshold;
			audioUnitConfig.frequencyBandIndex.resize(AUDIO_UNIT_NUM_BANDS);
			for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
			{
				audioUnitConfig.frequencyBandIndex.at(i) = TL::Configuration::getAudioUnitConfig().frequencyBandIndex[i];
			}

			const TL::AudioUnit::Error audioConfigError = TL::AudioUnit::setAudioUnitConfig(audioUnitConfig);
			if (audioConfigError != TL::AudioUnit::Error::OK)
			{
				TL::AudioUnit::end();
			}

			for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
			{
				const TL::AudioUnit::Error audioError = TL::AudioUnit::setPeakDetectorConfig(TL::Configuration::getAudioUnitConfig().peakDetectorConfig[i], i);
				if (audioError != TL::AudioUnit::Error::OK)
				{
					TL::AudioUnit::end();
				}
			}
		}
		else
		{
			TL::AudioUnit::end();
		}
	}

	TL::SystemInformation::HardwareInformation hwInfo = TL::SystemInformation::getHardwareInfo();
	hwInfo.mpu6050 = TL::MPU6050::isInitialized();
	hwInfo.ds18b20 = TL::DS18B20::isInitialized() ? TL::DS18B20::getNumSensors() : 0;
	hwInfo.bh1750 = TL::BH1750::isInitialized();
	hwInfo.audioUnit = TL::AudioUnit::isInitialized();
	TL::SystemInformation::setHardwareInfo(hwInfo);

	if (hwInfo.mpu6050 > 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Found MPU6050 motion sensor."));
	}
	if (hwInfo.ds18b20 > 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Found ") + hwInfo.ds18b20 + F(" DS18B20 temperature sensors."));
	}
	if (hwInfo.bh1750 > 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Found BH1750 light sensor."));
	}
	if (hwInfo.audioUnit > 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Found TesLight Audio Unit."));
	}
}

/**
 * @brief Initialize the LED manager. It has a dependency on the {@link TL::Configuration} which must be initialized
 * 		  beforehand. If it was not initialized before calling this function, the controller will reboot.
 */
void TesLight::initializeLedManager()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize LED manager."));
	const TL::LedManager::Error ledMangerInitError = TL::LedManager::begin();
	if (ledMangerInitError == TL::LedManager::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize LED manager. The configuration must be initialized beforehand. Rebooting."));
		TL::Updater::reboot(F("Failed to initialize LED manager."), 0);
	}
	else if (ledMangerInitError != TL::LedManager::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize LED manager due to unknown error."));
		TL::Updater::reboot(F("Failed to initialize LED manager."), 0);
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED manager initialized. Loading animations."));

	const TL::LedManager::Error ledManagerLoadError = TL::LedManager::reloadAnimations();
	if (ledManagerLoadError == TL::LedManager::Error::ERROR_INIT_LED_DRIVER)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. The LED driver could not be initialized for the LED configuration."));
	}
	else if (ledManagerLoadError == TL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. One of the animator types is unknown. Continuing without LEDs."));
	}
	else if (ledManagerLoadError == TL::LedManager::Error::ERROR_INVALID_FSEQ)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. The selected fseq file is invalid. Continuing without LEDs."));
	}
	else if (ledManagerLoadError == TL::LedManager::Error::ERROR_FILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. No fseq file with selected file id was found. Continuing without LEDs."));
	}
	else if (ledManagerLoadError == TL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration. The current configurationn does not match the configuration of the fseq file. Continuing without LEDs."));
	}
	else if (ledManagerLoadError != TL::LedManager::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LED configuration because of unknown error. Continuing without LEDs."));
	}
}

/**
 * @brief Initialize the motion sensor. It has a dependency on the {@link TL::Configuration} and {@link TL::MPU6050}.
 * 		  These must be initialized beforehand. If they were not initialized before calling this function, the motion
 * 		  sensor will not be available.
 */
void TesLight::initializeMotionSensor()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize motion sensor."));
	const TL::MotionSensor::Error motionSensorError = TL::MotionSensor::begin();
	if (motionSensorError == TL::MotionSensor::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Failed to initialize motion sensor because the configuration is not initialized."));
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor will not be available."));
		return;
	}
	else if (motionSensorError == TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No MPU6050 module could be found."));
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor will not be available."));
		return;
	}
	else if (motionSensorError != TL::MotionSensor::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error, motion sensor will not be available."));
		return;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor initialized."));
}

/**
 * @brief Initialize the light sensor. It has a dependency on the {@link TL::Configuration} which must be initialized
 * 		  beforehand. If it was not initialized before calling this function, the light sensor will not be available.
 */
void TesLight::initializeLightSensor()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize light sensor."));
	const TL::LightSensor::Error lightSensorError = TL::LightSensor::begin();
	if (lightSensorError == TL::LightSensor::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Failed to initialize light sensor because the configuration is not initialized."));
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Light sensor will not be available."));
		return;
	}
	else if (lightSensorError != TL::LightSensor::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error, light sensor will not be available."));
		return;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Light sensor initialized."));
}

/**
 * @brief Initialize the temperature sensor. It has a dependency on the {@link TL::DS18B20} which must be initialized
 * 		  beforehand. If it was not initialized before calling this function, the temperature sensor will not be available.
 */
void TesLight::initializeTemperatureSensor()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize temperature sensor."));
	const TL::TemperatureSensor::Error tempSensorError = TL::TemperatureSensor::begin();
	if (tempSensorError == TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No DS18B20 sensor could be found."));
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Temperature sensor will not be available."));
		return;
	}
	else if (tempSensorError != TL::TemperatureSensor::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Unknown error, temperature sensor will not be available."));
		return;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Temperature sensor initialized. Using ") + TL::DS18B20::getNumSensors() + F(" detected sensors."));
}

/**
 * @brief Initialize the fan controller. It has a dependency on the {@link TL::Configuration} which must be initialized beforehand.
 * 		  If it was not initialized before calling this function, the controller will reboot.
 */
void TesLight::initializeFan()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize fan controller."));
	const TL::Fan::Error fanError = TL::Fan::begin(FAN_PWM_PIN, FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	if (fanError == TL::Fan::Error::ERROR_CONFIG_UNAVAILABLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller because the configuration is not initialized. Rebooting."));
		TL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}
	else if (fanError == TL::Fan::Error::ERROR_TEMP_UNAVAILABLE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller because the temperature sensor is not initialized. Rebooting."));
		TL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}
	else if (fanError == TL::Fan::Error::ERROR_SETUP_PIN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller because the pwm channel could not be configured. Rebooting."));
		TL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}
	else if (fanError != TL::Fan::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize fan controller due to unknown error. Rebooting."));
		TL::Updater::reboot(F("Failed to initialize fan controller."), 0);
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Fan controller initialized."));
}

/**
 * @brief Initialize the WiFi manager. In case it fails, the controller will reboot.
 */
void TesLight::initializeWiFiManager()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize WiFi manager."));
	const TL::WiFiManager::Error wifiError = TL::WiFiManager::begin();
	if (wifiError == TL::WiFiManager::Error::ERROR_SET_MODE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set WiFi mode. Rebooting."));
		TL::Updater::reboot(F("Failed to set WiFi mode."), 0);
	}
	else if (wifiError != TL::WiFiManager::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi. Rebooting."));
		TL::Updater::reboot(F("Failed to start WiFi."), 0);
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi manager initialized."));
}

/**
 * @brief Initialize the web server manager.
 */
void TesLight::initializeWebServerManager()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize webserver."));
	TL::WebServerManager::begin(&SD, WEB_SERVER_PORT);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Webserver initialized."));
}

/**
 * @brief Initialize the rest api.
 */
void TesLight::initializeRestApi()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize REST API."));
	TL::ConnectionTestEndpoint::init(F("/api/"));
	TL::ConnectionTestEndpoint::begin();
	TL::SystemInformationEndpoint::init(F("/api/"));
	TL::SystemInformationEndpoint::begin();
	TL::SystemConfigurationEndpoint::init(F("/api/"));
	TL::SystemConfigurationEndpoint::begin();
	TL::LedConfigurationEndpoint::init(F("/api/"));
	TL::LedConfigurationEndpoint::begin();
	TL::WiFiConfigurationEndpoint::init(F("/api/"));
	TL::WiFiConfigurationEndpoint::begin();
	TL::FseqEndpoint::init(F("/api/"));
	TL::FseqEndpoint::begin(&SD);
	TL::LogEndpoint::init(F("/api/"));
	TL::LogEndpoint::begin(&SD);
	TL::UpdateEndpoint::init(F("/api/"));
	TL::UpdateEndpoint::begin(&SD);
	TL::ResetEndpoint::init(F("/api/"));
	TL::ResetEndpoint::begin(&SD);
	TL::MotionSensorEndpoint::init(F("/api/"));
	TL::MotionSensorEndpoint::begin();
	TL::AudioUnitConfigurationEndpoint::init(F("/api/"));
	TL::AudioUnitConfigurationEndpoint::begin();
	TL::UIConfigurationEndpoint::init(F("/api/"));
	TL::UIConfigurationEndpoint::begin();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("REST API initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Starting web server on port ") + WEB_SERVER_PORT + F("."));
	TL::WebServerManager::startServer();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Web server started."));
}

/**
 * @brief Initialize or reset timers.
 */
void TesLight::initializeTimers()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize/reset timers."));
	unsigned long mic = micros();
	TesLight::frameTimer = mic;
	TesLight::lightSensorTimer = mic;
	TesLight::motionSensorTimer = mic;
	TesLight::audioUnitTimer = mic;
	TesLight::temperatureTimer = mic;
	TesLight::statusTimer = mic;
	TesLight::statusPrintTimer = mic;
	TesLight::webServerTimer = mic;
	TesLight::frameCounter = 0;
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Timers initialized to ") + mic + F("."));
}

/**
 * @brief Check if a system update is available for installation. In case a update package was found,
 * 		  it will be installed automatically. This function will restart the controller and will not
 *		  return in case that a TUP was found and the update was installed.
 */
void TesLight::handleUpdate()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Check if TesLight update package is available for installation."));
	if (!TL::FileUtil::fileExists(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME))
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No update package found."));
		return;
	}
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight update package found. Installing system update."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Set fan speed to maximum during the update."));
	pinMode(FAN_PWM_PIN, OUTPUT);
	digitalWrite(FAN_PWM_PIN, HIGH);

	const TL::Updater::Error updateError = TL::Updater::install(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME);
	if (updateError == TL::Updater::Error::ERROR_UPDATE_FILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The update package could not be found."));
		return;
	}
	else if (updateError == TL::Updater::Error::ERROR_INVALID_FILE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The update package is invalid."));
		return;
	}
	else if (updateError == TL::Updater::Error::ERROR_CLEAN_FS)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The file system root could not be cleared. The MicroSD card might be corrupted. Trying to continue boot."));
		return;
	}
	else if (updateError == TL::Updater::Error::ERROR_UPDATE_UNPACK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The update package could not be unpacked. The MicroSD card might be corrupted. Trying to continue boot."));
		return;
	}
	else if (updateError == TL::Updater::Error::ERROR_FW_FILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The firmware file could not be found. Trying to continue boot."));
		return;
	}
	else if (updateError == TL::Updater::Error::ERROR_FW_FILE_EMPTY)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. The firmware file is empty. Trying to continue boot."));
		return;
	}
	else if (updateError == TL::Updater::Error::ERROR_OUT_OF_FLASH_MEMORY)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. Not enough flash memory for firmware update. Rebooting."));
		TL::Updater::reboot(F("Not enough flash memory for firmware update."), 0);
	}
	else if (updateError == TL::Updater::Error::ERROR_WRITE_FW_DATA)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. Could not write firmware data to flash memory. It might be required to reinstall the firmware via the USB port. Rebooting."));
		TL::Updater::reboot(F("Could not write firmware data to flash memory."), 0);
	}
	else if (updateError == TL::Updater::Error::ERROR_FINISH_FW_UPDATE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("System update failed. Could not finish firmware update. It might be required to reinstall the firmware via the USB port. Rebooting."));
		TL::Updater::reboot(F("Could not finish firmware update."), 0);
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System update installed successfully. Rebooting."));
	TL::Updater::reboot(F("System update reboot"), 0);
}

/**
 * @brief Create a WiFi access point for clients to connect to. In case the creation fails,
 * 		  the default configuration will be used to try to restore access to the controller.
 */
void TesLight::createtWiFiNetwork()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Creating WiFi network."));
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting WiFi access point."));
	uint8_t retry = 0;
	do
	{
		const TL::WiFiManager::Error wifiError = TL::WiFiManager::startAccessPoint(TL::Configuration::getWiFiConfig().accessPointSsid.c_str(), TL::Configuration::getWiFiConfig().accessPointPassword.c_str(), TL::Configuration::getWiFiConfig().accessPointChannel, false, TL::Configuration::getWiFiConfig().accessPointMaxConnections);
		if (wifiError == TL::WiFiManager::Error::OK)
		{
			TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi network created."));
			return;
		}
		else if (wifiError == TL::WiFiManager::Error::ERROR_START_AP)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
		}
		else if (wifiError == TL::WiFiManager::Error::ERROR_INVALID_SSID)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The SSID is invalid."));
		}
		else if (wifiError == TL::WiFiManager::Error::ERROR_INVALID_PW)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The password is invalid."));
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
		}

		if (retry == 0)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. Loading default settings and retry."));
			TL::Configuration::WiFiConfig wifiConfig = TL::Configuration::getWiFiConfig();
			wifiConfig.accessPointSsid = F(AP_DEFAULT_SSID);
			wifiConfig.accessPointPassword = F(AP_DEDAULT_PASSWORD);
			wifiConfig.accessPointChannel = AP_DEFAULT_CHANNEL;
			wifiConfig.accessPointHidden = AP_DEFAULT_HIDDEN;
			wifiConfig.accessPointMaxConnections = AP_DEFAULT_MAX_CONN;
			wifiConfig.wifiSsid = F(WIFI_DEFAULT_SSID);
			wifiConfig.wifiPassword = F(WIFI_DEFAULT_PASSWORD);
			TL::Configuration::setWiFiConfig(wifiConfig);
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
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
bool TesLight::checkTimer(unsigned long &timer, unsigned long cycleTime)
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
void TesLight::run()
{
	// Handle the pixel rendering and LED output
	if (TesLight::checkTimer(TesLight::frameTimer, TL::LedManager::getFrameInterval()))
	{
		TL::LedManager::render();
		TL::LedManager::show(portMAX_DELAY);
		TL::LedManager::waitShow(portMAX_DELAY);
		TesLight::frameCounter++;
		TesLight::ledPowerCounter += TL::LedManager::getLedPowerDraw();
	}

	// Handle the light sensor
	if (TesLight::checkTimer(TesLight::lightSensorTimer, TesLight::lightSensorInterval) && TL::LightSensor::isInitialized())
	{
		float brightness;
		const TL::LightSensor::Error lightSensorError = TL::LightSensor::getBrightness(brightness);
		if (lightSensorError == TL::LightSensor::Error::OK)
		{
			TL::LedManager::setAmbientBrightness(brightness);
			TesLight::lightSensorInterval = LIGHT_SENSOR_INTERVAL;
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read light sensor data. Delaying next read by 1s."));
			TL::LedManager::setAmbientBrightness(1.0f);
			TesLight::lightSensorInterval = 1000000;
		}
	}

	// Handle the motion sensor
	if (TesLight::checkTimer(TesLight::motionSensorTimer, TesLight::motionSensorInterval) && TL::MotionSensor::isInitialized())
	{
		const TL::MotionSensor::Error motionSensorError = TL::MotionSensor::run();
		if (motionSensorError == TL::MotionSensor::Error::OK)
		{
			TL::LedManager::setMotionSensorData(TL::MotionSensor::getMotion());
			TesLight::motionSensorInterval = MOTION_SENSOR_INTERVAL;
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read motion sensor data. Delaying next read by 1s"));
			TesLight::motionSensorInterval = 1000000;
		}
	}

	// Handle the audio unit
	if (TesLight::checkTimer(TesLight::audioUnitTimer, TesLight::audioUnitInterval) && TL::AudioUnit::isInitialized())
	{
		TL::AudioUnit::AudioAnalysis audioAnalysis;
		const TL::AudioUnit::Error audioError = TL::AudioUnit::getAudioAnalysis(audioAnalysis);
		if (audioError == TL::AudioUnit::Error::OK)
		{
			TL::LedManager::setAudioAnalysis(audioAnalysis);
			TesLight::audioUnitInterval = AUDIO_UNIT_INTERVAL;
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read audio analysis data. Delaying next read by 1s"));
			TesLight::audioUnitInterval = 1000000;
		}
	}

	// Handle the fan controller
	if (TesLight::checkTimer(TesLight::temperatureTimer, FAN_INTERVAL))
	{
		const TL::Fan::Error fanError = TL::Fan::run(static_cast<TL::Fan::FanMode>(TL::Configuration::getSystemConfig().fanMode));
		if (fanError == TL::Fan::Error::ERROR_TEMP_UNAVAILABLE)
		{
			TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Failed to update cooling fan because the temperature could not be read. Using fallback."));
		}
		else if (fanError != TL::Fan::Error::OK)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to update cooling fan."));
		}

		TL::SystemInformation::HardwareInformation hwInfo = TL::SystemInformation::getHardwareInfo();
		hwInfo.fanSpeed = TL::Fan::getPwmValue();
		TL::SystemInformation::setHardwareInfo(hwInfo);
	}

	// Update the soc, led and hardware information
	if (TesLight::checkTimer(TesLight::statusTimer, STATUS_INTERVAL))
	{
		// Update the SOC information
		TL::SystemInformation::updateSocInfo();

		// Update LED related information
		TL::SystemInformation::TLInformation tlInfo = TL::SystemInformation::getTesLightInfo();
		tlInfo.fps = frameCounter / (STATUS_INTERVAL / 1000000.0f);
		tlInfo.ledCount = TL::LedManager::getLedCount();
		tlInfo.hiddenLedCount = TL::LedManager::getHiddenLedCount();
		TL::SystemInformation::setTesLightInfo(tlInfo);

		// Update regulator related information
		TL::SystemInformation::HardwareInformation hwInfo = TL::SystemInformation::getHardwareInfo();
		hwInfo.regulatorPowerDraw = ledPowerCounter / frameCounter;
		hwInfo.regulatorCurrentDraw = hwInfo.regulatorPowerDraw / hwInfo.regulatorVoltage;
		if (TL::TemperatureSensor::isInitialized())
		{
			if (TL::TemperatureSensor::getMaxTemperature(hwInfo.regulatorTemperature) != TL::TemperatureSensor::Error::OK)
			{
				hwInfo.regulatorTemperature = 0.0f;
			}
		}
		else
		{
			hwInfo.regulatorTemperature = 0.0f;
		}
		TL::SystemInformation::setHardwareInfo(hwInfo);

		TesLight::frameCounter = 0;
		TesLight::ledPowerCounter = 0.0f;
	}

	// Print the system status
	if (TesLight::checkTimer(TesLight::statusPrintTimer, STATUS_PRINT_INTERVAL))
	{
		const TL::SystemInformation::SocInfo socInfo = TL::SystemInformation::getSocInfo();
		const TL::SystemInformation::TLInformation tlInfo = TL::SystemInformation::getTesLightInfo();
		const TL::SystemInformation::HardwareInformation hwInfo = TL::SystemInformation::getHardwareInfo();
		TL::Logger::log(
			TL::Logger::LogLevel::INFO,
			SOURCE_LOCATION,
			(String)F("LED Driver: ") + tlInfo.fps + F("FPS   ") +
				F("Average Power: ") + hwInfo.regulatorPowerDraw + F("W   ") +
				F("Average Current: ") + hwInfo.regulatorCurrentDraw + F("A   ") +
				F("Temperature: ") + hwInfo.regulatorTemperature + F("°C   ") +
				F("Fan: ") + hwInfo.fanSpeed / 2.55f + F("%   ") +
				F("Heap (free): ") + socInfo.freeHeap + F("kB"));
	}

	// Handle web server requests
	if (TesLight::checkTimer(TesLight::webServerTimer, WEB_SERVER_INTERVAL))
	{
		// Stop the watchdog to not trigger it during longer requests
		TL::WatchDog::deleteTaskWatchdog();

		// Run the server and measure execution time
		unsigned long start = millis();
		TL::WebServerManager::handleRequest();
		unsigned long executionTime = millis() - start;
		if (executionTime > 2000)
		{
			// When the execution time was longer than 2s, reset the timers
			initializeTimers();
		}

		// Start the watchdog again
		TL::WatchDog::initializeTaskWatchdog();
	}

	// Reset the task watchdog timer
	TL::WatchDog::resetTaskWatchdog();
}
