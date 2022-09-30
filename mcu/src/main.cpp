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

#include "configuration/SystemConfiguration.h"

#include "logging/Logger.h"
#include "configuration/Configuration.h"

#include "led/LedManager.h"

#include "sensor/LightSensor.h"
#include "sensor/MotionSensor.h"

#include "wifi/WiFiManager.h"
#include "server/WebServer.h"
#include "server/ConnectionTestEndpoint.h"
#include "server/SystemConfigurationEndpoint.h"
#include "server/LedConfigurationEndpoint.h"
#include "server/WiFiConfigurationEndpoint.h"
#include "server/FseqEndpoint.h"

#define MAX_ULONG_VALUE 0xffffffffffffffff

TesLight::Configuration *configuration = nullptr;
TesLight::LedManager *ledManager = nullptr;
TesLight::LightSensor *lightSensor = nullptr;
TesLight::MotionSensor *motionSensor = nullptr;
TesLight::WiFiManager *wifiManager = nullptr;
TesLight::WebServer *webServer = nullptr;

// Timer
uint64_t ledTimer = 0;
uint64_t lightSensorTimer = 0;
uint64_t motionSensorTimer = 0;
uint64_t fpsTimer = 0;
uint16_t ledFrameCounter = 0;

// Flags
bool systemConfigChangedFlag = false;
bool ledConfigChangedFlag = false;
bool wifiConfigChangedFlag = false;

// Initialization functions
void printLogo();
bool initializeLogger(bool sdLogging);
bool initializeSdCard();
bool initializeConfiguration();
void initializeLedManager();
void initializeLightSensor();
bool initializeMotionSensor();
void initializeWiFiManager();
void initializeWebServer();
void initializeRestApi();
void initializeTimers();
void stop();

// WiFi functions
bool createtWiFiNetwork();

// Callback functions for the rest endpoints
void systemConfigChanged();
void ledConfigChanged();
void wifiConfigChanged();

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
	Serial.println(F("Firmware version 0.9.5 (beta)"));
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
	return SD.begin(SD_CS_PIN);
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
 * @brief Initialize the {@link TesLight::LedManager} to handle the LEDs and animators.
 */
void initializeLedManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize LED Manager."));
	ledManager = new TesLight::LedManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED Manager initialized."));
}

/**
 * @brief Initialize the {@link TesLight::LightSensor} to read the status of the car's lights.
 */
void initializeLightSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initializing light sensor at pin ") + String(LIGHT_SENSOR_PIN) + F(" with buffer size ") + String(LIGHT_SENSOR_BUFFER_SIZE) + F("."));
	const TesLight::Configuration::SystemConfig systemConfig = configuration->getSystemConfig();
	lightSensor = new TesLight::LightSensor(LIGHT_SENSOR_PIN, LIGHT_SENSOR_BUFFER_SIZE, systemConfig.lightSensorMode, systemConfig.lightSensorThreshold, systemConfig.lightSensorMinValue, systemConfig.lightSensorMaxValue);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light sensor initialized."));
}

/**
 * @brief Initialize the motion sensor. In case the initialization fails the motion sensor pointer will be set to {@code nullptr}.
 * @return true when successful
 * @return false when there was an error
 */
bool initializeMotionSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initializing motion sensor at ") + String(MOTION_SENSOR_I2C_ADDRESS) + F(" with buffer size ") + String(MOTION_SENSOR_BUFFER_SIZE) + F("."));
	motionSensor = new TesLight::MotionSensor(MOTION_SENSOR_I2C_ADDRESS, SDA_PIN, SCL_PIN, MOTION_SENSOR_BUFFER_SIZE);
	if (motionSensor->begin())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Motion sensor initialized."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initializing motion sensor."));
		delete motionSensor;
		motionSensor = nullptr;
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
void initializeWebServer()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize webserver."));
	webServer = new TesLight::WebServer(WEB_SERVER_PORT, &SD, WEB_SERVER_STATIC_CONTENT);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Webserver initialized."));
}

/**
 * @brief Initialize the REST api.
 */
void initializeRestApi()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize REST API."));
	TesLight::ConnectionTestEndpoint::init(webServer, "/api/");
	TesLight::ConnectionTestEndpoint::begin();
	TesLight::SystemConfigurationEndpoint::init(webServer, "/api/");
	TesLight::SystemConfigurationEndpoint::begin(configuration, systemConfigChanged);
	TesLight::LedConfigurationEndpoint::init(webServer, "/api/");
	TesLight::LedConfigurationEndpoint::begin(configuration, ledConfigChanged);
	TesLight::WiFiConfigurationEndpoint::init(webServer, "/api/");
	TesLight::WiFiConfigurationEndpoint::begin(configuration, wifiConfigChanged);
	TesLight::FseqEndpoint::init(webServer, "/api/");
	TesLight::FseqEndpoint::begin(&SD);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("REST API initialized."));
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
	fpsTimer = micros();
}

/**
 * @brief Stop the program.
 */
void stop(const String reason)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Programm stopped because: ") + reason);
	while (1)
		;
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
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize SD card."));
		stop(F("Failed to initialize SD card."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Switching to SD card logger."));
	delay(500);
	if (initializeLogger(true))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Switched to SD card logger."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to switch to SD card logger."));
		stop(F("Failed to switch to SD card logger."));
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
	TesLight::Logger::setMinLogLevel(systemConfig.logLevel);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Log level updated from configuration."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize LED Manager."));
	initializeLedManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED Manager initialized."));

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
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to initialize motion sensor. Continue without motion sensor data."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize WiFiManager."));
	initializeWiFiManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi manager initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting Webserver."));
	initializeWebServer();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Webserver started on port ") + WEB_SERVER_PORT + F("."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initializing REST api."));
	initializeRestApi();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("REST api initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Load LEDs and animators from configuration using the LED Manager."));
	if (ledManager->loadFromConfiguration(configuration))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LEDs and animators loaded."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load LEDs and animators. Continue without rendering LEDs."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Creating to WiFi network."));
	if (createtWiFiNetwork())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi Network created."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to create WiFi network. Continuing without WiFi network. The REST API might be inaccessible."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize timers."));
	initializeTimers();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Timers initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight initialized successfully, going into work mode."));
}

/**
 * @brief Main loop runs as long as the controller is active.
 */
void loop()
{
	// Handle the LEDs
	if (micros() - ledTimer >= LED_CYCLE_TIME && ledTimer != MAX_ULONG_VALUE)
	{
		ledTimer = micros() - (micros() - ledTimer - LED_CYCLE_TIME);
		ledManager->render();
		ledManager->show();
		ledFrameCounter++;
	}

	// Handle the light sensor
	if (micros() - lightSensorTimer >= LIGHT_SENSOR_CYCLE_TIME && lightSensorTimer != MAX_ULONG_VALUE)
	{
		lightSensorTimer = micros() - (micros() - lightSensorTimer - LIGHT_SENSOR_CYCLE_TIME);
		if (lightSensor != nullptr)
		{
			const float brightness = lightSensor->getBrightness();
			ledManager->setAmbientBrightness(brightness);
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to read light sensor data. Light sensor is disabled now."));
			lightSensorTimer = MAX_ULONG_VALUE;
		}
	}

	// Handle the motion sensor
	if (micros() - motionSensorTimer >= MOTION_SENSOR_CYCLE_TIME && motionSensorTimer != MAX_ULONG_VALUE)
	{
		motionSensorTimer = micros() - (micros() - motionSensorTimer - MOTION_SENSOR_CYCLE_TIME);
		if (motionSensor == nullptr || !motionSensor->readData())
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to read motion sensor data. Motion sensor is disabled now."));
			motionSensorTimer = MAX_ULONG_VALUE;
		}
	}

	// Measure the FPS
	if (micros() - fpsTimer >= FPS_CYCLE_TIME)
	{
		fpsTimer = micros() - (micros() - fpsTimer - FPS_CYCLE_TIME);
		ledFrameCounter /= FPS_CYCLE_TIME / 1000000;
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("LEDs running at an average of ") + ledFrameCounter + F(" FPS"));
		ledFrameCounter = 0;
	}

	// Handle System configuration updates when the flag is set
	if (systemConfigChangedFlag)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System configuration has changed. Updating system configuration."));
		systemConfigChangedFlag = false;
		const TesLight::Configuration::SystemConfig systemConfig = configuration->getSystemConfig();
		TesLight::Logger::setMinLogLevel(systemConfig.logLevel);
		if (lightSensor != nullptr)
		{
			lightSensor->setThreshold(systemConfig.lightSensorThreshold);
			lightSensor->setMinValue(systemConfig.lightSensorMinValue);
			lightSensor->setMaxValue(systemConfig.lightSensorMaxValue);
			lightSensor->setLightSensorMode(systemConfig.lightSensorMode);
		}
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System configuration updated."));
	}

	// Handle LED configuration updates when the flag is set
	if (ledConfigChangedFlag)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED configuration has changed. Reload LEDs and animators using the LED Manager."));
		ledConfigChangedFlag = false;
		if (ledManager->loadFromConfiguration(configuration))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LEDs and animators reloaded."));
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to reload LEDs and animators. Continue without rendering LEDs."));
		}
	}

	// Handle WiFi configuration updates when the flag is set
	if (wifiConfigChangedFlag)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration has changed. Updating WiFi configuration."));
		wifiConfigChangedFlag = false;
		if (createtWiFiNetwork())
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration updated."));
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to create WiFi network. Continuing without WiFi network. The REST API might be inaccessible."));
		}
	}
}

/**
 * @brief Is called by the {@link TesLight::Configuration::SystemConfigurationEndpoint} when the system configuration was updated.
 * Since it's called async by the WebServer it should be used with care. The actual update should only happen in the main loop.
 */
void systemConfigChanged()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("The system configuration was updated, setting flag."));
	systemConfigChangedFlag = true;
}

/**
 * @brief Is called by the {@link TesLight::Configuration::LedConfigurationEndpoint} when the LED configuration was updated.
 * Since it's called async by the WebServer it should be used with care. The actual update should only happen in the main loop.
 */
void ledConfigChanged()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("The LED configuration was updated, setting flag."));
	ledConfigChangedFlag = true;
}

/**
 * @brief Is called by the {@link TesLight::Configuration::WiFiConfigurationEndpoint} when the WiFi configuration was updated.
 * Since it's called async by the WebServer it should be used with care. Actual update should only happen in main loop.
 */
void wifiConfigChanged()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("The WiFi configuration was updated, setting flag."));
	wifiConfigChangedFlag = true;
}
