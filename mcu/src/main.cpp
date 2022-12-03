/**
 * @file main.cpp
 * @author TheRealKasumi
 * @brief Entry point, initialization and main loop of the program.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include <SD.h>
#include <esp_task_wdt.h>
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"
#include "configuration/Configuration.h"
#include "hardware/FanController.h"
#include "led/LedManager.h"
#include "sensor/TemperatureSensor.h"
#include "sensor/LightSensor.h"
#include "sensor/MotionSensor.h"
#include "wifi/WiFiManager.h"
#include "server/WebServerManager.h"
#include "server/ConnectionTestEndpoint.h"
#include "server/SystemConfigurationEndpoint.h"
#include "server/LedConfigurationEndpoint.h"
#include "server/WiFiConfigurationEndpoint.h"
#include "server/FseqEndpoint.h"
#include "server/LogEndpoint.h"
#include "server/UpdateEndpoint.h"
#include "server/ResetEndpoint.h"
#include "server/MotionSensorEndpoint.h"
#include "util/FileUtil.h"
#include "update/Updater.h"

TesLight::Configuration *configuration = nullptr;
TesLight::FanController *fanController = nullptr;
TesLight::LedManager *ledManager = nullptr;
TesLight::TemperatureSensor *temperatureSensor = nullptr;
TesLight::LightSensor *lightSensor = nullptr;
TesLight::MotionSensor *motionSensor = nullptr;
TesLight::WiFiManager *wifiManager = nullptr;
TesLight::WebServerManager *webServerManager = nullptr;

// Timer
unsigned long ledTimer = 0;
unsigned long lightSensorTimer = 0;
unsigned long motionSensorTimer = 0;
unsigned long webServerTimer = 0;
unsigned long statusTimer = 0;
unsigned long temperatureTimer = 0;
uint16_t ledFrameCounter = 0;
float ledPowerCounter = 0.0f;

// Initialization functions
void printLogo();
bool initializeLogger(bool sdLogging);
bool initializeSdCard();
bool initializeConfiguration();
void intializeI2C();
void initializeFanController();
void initializeLedManager();
void initializeTemperatureSensor();
void initializeLightSensor();
bool initializeMotionSensor();
void initializeWiFiManager();
void initializeWebServerManager();
void initializeRestApi();
void initializeTimers();
bool checkTimer(unsigned long &timer, unsigned long cycleTime);

// System update
bool updateAvilable();
void handleUpdate();

// WiFi functions
bool createtWiFiNetwork();

// Callback functions for the rest endpoints
bool applySystemConfig();
bool applyLedConfig();
bool applyWifiConfig();

/**
 * @brief Print the TesLight logo and FW version.
 */
void printLogo()
{
	Serial.println();
	Serial.println(F("████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗"));
	Serial.println(F("╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝"));
	Serial.println(F("   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║   "));
	Serial.println(F("   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║   "));
	Serial.println(F("   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║   "));
	Serial.println(F("   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   "));
	Serial.println(F("Firmware version 1.0.0"));
	Serial.println();
}

/**
 * @brief Initialize the serial and SD card logger.
 * @return true when initialized successfully
 * @return false when error
 */
bool initializeLogger(bool sdLogging)
{
	TesLight::Logger::setMinLogLevel(TesLight::Logger::LogLevel::INFO);
	if (!sdLogging)
	{
		return TesLight::Logger::begin(SERIAL_BAUD_RATE);
	}
	else
	{
		return TesLight::Logger::begin(SERIAL_BAUD_RATE, &SD, LOG_FILE_NAME);
	}
}

/**
 * @brief Initialize the SD card.
 * @return true when initialized successfully
 * @return false when error
 */
bool initializeSdCard()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize MicroSD card."));
	return SD.begin(SD_CS_PIN, SPI, SD_SPI_SPEED, SD_MOUNT_POINT, SD_MAX_FILES, false);
}

/**
 * @brief Initialize the configuration and load it from the SD card or default.
 * @return true when the configuration was loaded
 * @return false when there was an error
 */
bool initializeConfiguration()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize runtime configuration from MicroSD card."));
	configuration = new TesLight::Configuration(&SD, CONFIGURATION_FILE_NAME);
	if (configuration->load() && configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Runtime configuration initialized."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize runtime configuration from MicroSD card."));
		return false;
	}
}

/**
 * @brief Initialize the I²C bus.
 */
void intializeI2C()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize I²C bus."));
	Wire.begin((int)IIC_SDA_PIN, (int)IIC_SCL_PIN);
}

/**
 * @brief Initialize the {@link TesLight::FanController} to handle the cooling fan.
 */
void initializeFanController()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize fan controller."));
	fanController = new TesLight::FanController(FAN_PWM_PIN, configuration);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Fan controller initialized."));
}

/**
 * @brief Initialize the {@link TesLight::LedManager} to handle the LEDs and animators.
 */
void initializeLedManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize LED manager."));
	ledManager = new TesLight::LedManager(configuration);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED manager initialized."));
}

/**
 * @brief Initialize the {@link TesLight::TemperatureSensor} to read all detected sensors on the bus.
 */
void initializeTemperatureSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize temperatuer sensor."));
	temperatureSensor = new TesLight::TemperatureSensor();
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("There are ") + temperatureSensor->getNumSensors() + F(" sensors on the OneWire bus."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Temperature sensor initialized."));
}

/**
 * @brief Initialize the {@link TesLight::LightSensor} to read the status of the car's lights.
 */
void initializeLightSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize light sensor."));
	lightSensor = new TesLight::LightSensor(configuration);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light sensor initialized."));
}

/**
 * @brief Initialize the motion sensor.
 * @return true when successful
 * @return false when there was an error
 */
bool initializeMotionSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initialize motion sensor at with I²C address") + String(IIC_ADDRESS_MPU6050) + F("."));
	motionSensor = new TesLight::MotionSensor(IIC_ADDRESS_MPU6050, configuration);
	if (motionSensor->begin())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Motion sensor initialized."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize motion sensor."));
		return false;
	}
}

/**
 * @brief Initialize the wifi manager.
 */
void initializeWiFiManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize WiFi manager."));
	wifiManager = new TesLight::WiFiManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi manager initialized."));
}

/**
 * @brief Initialize the webserver to serve static files.
 */
void initializeWebServerManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize webserver."));
	webServerManager = new TesLight::WebServerManager(WEB_SERVER_PORT, &SD, WEB_SERVER_STATIC_CONTENT);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Webserver initialized."));
}

/**
 * @brief Initialize the REST api.
 */
void initializeRestApi()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize REST API."));
	TesLight::ConnectionTestEndpoint::init(webServerManager, F("/api/"));
	TesLight::ConnectionTestEndpoint::begin();
	TesLight::SystemConfigurationEndpoint::init(webServerManager, F("/api/"));
	TesLight::SystemConfigurationEndpoint::begin(configuration, applySystemConfig);
	TesLight::LedConfigurationEndpoint::init(webServerManager, F("/api/"));
	TesLight::LedConfigurationEndpoint::begin(configuration, applyLedConfig);
	TesLight::WiFiConfigurationEndpoint::init(webServerManager, F("/api/"));
	TesLight::WiFiConfigurationEndpoint::begin(configuration, applyWifiConfig);
	TesLight::FseqEndpoint::init(webServerManager, F("/api/"));
	TesLight::FseqEndpoint::begin(&SD, configuration);
	TesLight::LogEndpoint::init(webServerManager, F("/api/"));
	TesLight::LogEndpoint::begin(&SD);
	TesLight::UpdateEndpoint::init(webServerManager, F("/api/"));
	TesLight::UpdateEndpoint::begin(&SD);
	TesLight::ResetEndpoint::init(webServerManager, F("/api/"));
	TesLight::ResetEndpoint::begin(&SD);
	TesLight::MotionSensorEndpoint::init(webServerManager, F("/api/"));
	TesLight::MotionSensorEndpoint::begin(configuration, motionSensor);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("REST API initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Starting web server."));
	webServerManager->begin();
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Web server started."));
}

/**
 * @brief Initialize the timers.
 */
void initializeTimers()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize timers."));
	ledTimer = micros();
	lightSensorTimer = micros();
	motionSensorTimer = micros();
	webServerTimer = micros();
	statusTimer = micros();
	temperatureTimer = micros();
	ledFrameCounter = 0;
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Timers initialized."));
}

/**
 * @brief Check if a timer expired.
 * @param timer timer value
 * @param cycleTime cycle time of the timer
 * @return true when the timer expired
 * @return false when the timer is not expired yet
 */
bool checkTimer(unsigned long &timer, unsigned long cycleTime)
{
	if (micros() - timer >= cycleTime)
	{
		timer += cycleTime;
		return true;
	}
	return false;
}

/**
 * @brief Check if the update file is available.
 * @return true when update file is available
 * @return false when there is no update file
 */
bool updateAvilable()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Check if update package file is available."));
	if (TesLight::FileUtil::fileExists(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Update package file was found."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("No update package found."));
		return false;
	}
}

/**
 * @brief Install the system update from a TUP file. This function will not return.
 */
void handleUpdate()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Install system update from update package."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Set fan speed to maximum during the update."));
	pinMode(FAN_PWM_PIN, OUTPUT);
	digitalWrite(FAN_PWM_PIN, HIGH);

	if (TesLight::Updater::install(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update installed successfully. Rebooting. Good luck ;) !"));
		TesLight::Updater::reboot(F("Update Success"));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to install system update! Trying to continue."));
	}
}

/**
 * @brief Create a WiFi access point and connect to the wifi network.
 * @return true when successful
 * @return false when there was an error
 */
bool createtWiFiNetwork()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Creating WiFi network."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Starting WiFi access point."));
	if (wifiManager->startAccessPoint(configuration->getWiFiConfig().accessPointSsid.c_str(), configuration->getWiFiConfig().accessPointPassword.c_str(), configuration->getWiFiConfig().accessPointChannel, false, configuration->getWiFiConfig().accessPointMaxConnections))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi AP started and running."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. Please check your configuration."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi network created."));
	return true;
}

/**
 * @brief Initialize the software and hardware.
 */
void setup()
{
	initializeLogger(false);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight booting up."));
	printLogo();

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize SD card."));
	if (initializeSdCard())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("SD card initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize SD card. Try reboot."));
		TesLight::Updater::reboot("SD mount failure.");
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Switching to SD card logger."));
	delay(500);
	if (initializeLogger(true))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Switched to SD card logger."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to switch to SD card logger. Try reboot."));
		TesLight::Updater::reboot("SD logger failure.");
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Check if system update is available."));
	if (updateAvilable())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System update package was found. Installing..."));
		handleUpdate();
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No available system update found."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize, load and save configuration."));
	if (initializeConfiguration())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Runtime configuration loaded."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load configuration. Loading defaults and continue."));
		configuration->loadDefaults();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Updating log level from configuration."));
	const TesLight::Configuration::SystemConfig systemConfig = configuration->getSystemConfig();
	TesLight::Logger::setMinLogLevel((TesLight::Logger::LogLevel)systemConfig.logLevel);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Log level updated from configuration."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize I²C bus."));
	intializeI2C();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("I²C bus initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize Fan Controller."));
	initializeFanController();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Fan Controller initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize LED Manager."));
	initializeLedManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED Manager initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize temperature sensor."));
	initializeTemperatureSensor();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Temperature sensor initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize light sensor."));
	initializeLightSensor();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Light sensor initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize motion sensor."));
	if (initializeMotionSensor())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize motion sensor. Continue without motion sensor data."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize WiFiManager."));
	initializeWiFiManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi manager initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting Webserver."));
	initializeWebServerManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Webserver started on port ") + WEB_SERVER_PORT + F("."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize REST api."));
	initializeRestApi();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("REST api initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Load LEDs and animators from configuration using the LED Manager."));
	if (ledManager->reloadAnimations())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LEDs and animators loaded."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load LEDs and animators. Continue without rendering LEDs."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Creating to WiFi network."));
	if (createtWiFiNetwork())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi Network created."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create WiFi network. Continuing without WiFi network. The REST API might be inaccessible."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize timers."));
	initializeTimers();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Timers initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize watchdog."));
	esp_task_wdt_init(WATCHDOG_RESET_TIME, true);
	esp_task_wdt_add(NULL);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Watchdog initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight initialized successfully, going into work mode."));
}

/**
 * @brief Main loop runs as long as the controller is active.
 */
void loop()
{
	// Handle the LEDs
	if (checkTimer(ledTimer, ledManager->getTargetFrameTime()))
	{
		ledManager->render();
		ledManager->show();
		ledFrameCounter++;
		ledPowerCounter += ledManager->getLedPowerDraw();
	}

	// Handle the light sensor
	if (checkTimer(lightSensorTimer, LIGHT_SENSOR_CYCLE_TIME))
	{
		float brightness;
		if (ledManager->getAmbientBrightness(brightness) && lightSensor->getBrightness(brightness, motionSensor))
		{
			ledManager->setAmbientBrightness(brightness);
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read light sensor data. Delaying next read by 10s."));
			lightSensorTimer += 10000000;
		}
	}

	// Handle the motion sensor
	if (checkTimer(motionSensorTimer, MOTION_SENSOR_CYCLE_TIME))
	{
		if (motionSensor->run())
		{
			ledManager->setMotionSensorData(motionSensor->getMotion());
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read motion sensor data. Delaying next read by 10s"));
			motionSensorTimer += 10000000;
		}
	}

	// Handle web server requests
	if (checkTimer(webServerTimer, WEB_SERVER_CYCLE_TIME))
	{
		esp_task_wdt_delete(NULL);
		webServerManager->handleRequest();
		esp_task_wdt_init(WATCHDOG_RESET_TIME, true);
		esp_task_wdt_add(NULL);
	}

	// Measure the FPS
	if (checkTimer(statusTimer, STATUS_CYCLE_TIME))
	{
		const float fps = (float)ledFrameCounter / (STATUS_CYCLE_TIME / 1000000);
		const float powerDraw = ledPowerCounter / ledFrameCounter;
		float temperature;
		if (!temperatureSensor->getMaxTemperature(temperature))
		{
			temperature = 0.0f;
		}
		ledFrameCounter = 0;
		ledPowerCounter = 0.0f;
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("LEDs: ") + fps + F("FPS      Average Power: ") + powerDraw + F("W      Regulators: ") + temperature + F("°C"));
	}

	// Handle the temperature measurement and fan controller
	if (checkTimer(temperatureTimer, TEMP_CYCLE_TIME))
	{
		float temp;
		if (temperatureSensor->getMaxTemperature(temp))
		{
			ledManager->setRegulatorTemperature(temp);
			fanController->setTemperature(temp > -75.0f ? temp : configuration->getSystemConfig().fanMaxTemperature);
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read temperature sensor. Delaying next read by 5s"));
			temperatureTimer += 5000000;
		}
	}

	// Reset the watchdog timer
	esp_task_wdt_reset();
}

/**
 * @brief Callback function is called via an REST endpoint when the system configuration was updated.
 * @return true when the configuration was applied successfully
 * @return false when there was an error applying the configuration
 */
bool applySystemConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System configuration has changed. Updating system configuration."));
	const TesLight::Configuration::SystemConfig systemConfig = configuration->getSystemConfig();
	TesLight::Logger::setMinLogLevel((TesLight::Logger::LogLevel)systemConfig.logLevel);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System configuration updated."));
	initializeTimers();
	return true;
}

/**
 * @brief Callback function is called via an REST endpoint when the LED configuration was updated.
 * @return true when the configuration was applied successfully
 * @return false when there was an error applying the configuration
 */
bool applyLedConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED configuration has changed. Reload LEDs and animators using the LED Manager."));
	if (ledManager->reloadAnimations())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LEDs and animators reloaded."));
		initializeTimers();
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to reload LEDs and animators. Continue without rendering LEDs."));
		initializeTimers();
		return false;
	}
}

/**
 * @brief Callback function is called via an REST endpoint when the WiFi configuration was updated.
 * @return true when the configuration was applied successfully
 * @return false when there was an error applying the configuration
 */
bool applyWifiConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration has changed. Updating WiFi configuration."));
	if (createtWiFiNetwork())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration updated."));
		initializeTimers();
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create WiFi network. Continuing without WiFi network. The REST API might be inaccessible."));
		initializeTimers();
		return false;
	}
}
