/**
 * @file main.cpp
 * @author TheRealKasumi
 * @brief Entry point, initialization and main loop of the program.
 * @version 0.0.1
 * @date 2022-09-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include <SD.h>

#include "configuration/SystemConfiguration.h"

#include "logging/Logger.h"
#include "configuration/Configuration.h"
#include "led/driver/LedDriver.h"
#include "sensor/SwitchSensor.h"
#include "sensor/MotionSensor.h"
#include "wifi/WiFiManager.h"

#include "server/WebServer.h"
#include "server/ConnectionTestEndpoint.h"
#include "server/WiFiConfigurationEndpoint.h"
#include "server/LedConfigurationEndpoint.h"

#include "led/animator/RainbowAnimator.h"
#include "led/animator/GradientAnimator.h"
#include "led/animator/StaticColorAnimator.h"

#define MAX_ULONG_VALUE 0xffffffffffffffff

TesLight::Configuration *configuration = nullptr;
TesLight::LedDriver *ledDriver[NUM_LED_DRIVERS] = {nullptr};
TesLight::LedAnimator *ledAnimator[NUM_LED_DRIVERS] = {nullptr};
TesLight::SwitchSensor *switchSensor = nullptr;
TesLight::MotionSensor *motionSensor = nullptr;
TesLight::WiFiManager *wifiManager = nullptr;
TesLight::WebServer *webServer = nullptr;

uint64_t ledTimer = 0;
uint64_t switchSensorTimer = 0;
uint64_t motionSensorTimer = 0;
uint64_t fpsTimer = 0;
uint16_t ledFrameCounter = 0;

bool wifiConfigChangedFlag = false;
bool ledConfigChangedFlag = false;

// Function declarations
void printLogo();
bool initializeLogger(bool sdLogging);
bool initializeSdCard();
bool initializeConfiguration();
bool initializeLedDrivers();
void initializeSwitchSensor();
bool initializeMotionSensor();
void initializeWiFiManager();
bool createtWiFiNetwork();
void initializeWebServer();
void initializeRestApi();
void stop();
void wifiConfigChanged();
void ledConfigChanged();

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
	Serial.println(F("Firmware version 0.0.1 (beta)"));
	Serial.println();
}

/**
 * @brief Initialize the serial and SD card logger.
 * @return true when initialized successfully
 * @return false when error
 */
bool initializeLogger(bool sdLogging)
{
	TesLight::Logger::setMinLogLevel(MIN_LOG_LEVEl);
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
	return SD.begin(SD_CS_PIN);
}

/**
 * @brief Initialize the configuration and load it from the SD card.
 * @return true when the configuration was loaded
 * @return false when there was an error
 */
bool initializeConfiguration()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeConfiguration"), F("Initialize runtime configuration..."));

	configuration = new TesLight::Configuration(&SD, CONFIGURATION_FILE_NAME);
	if (configuration->load() && configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeConfiguration"), F("Runtime configuration initialized."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("main.cpp:initializeConfiguration"), F("Failed to initialize runtime configuration."));
		return false;
	}
}

/**
 * @brief Initialize new led drivers and deletes old ones. In case the initialization fails the driver pointers will be set to {@code nullptr}.
 * @return true when initialized successfully
 * @return false when error
 */
bool initializeLedDrivers()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), F("Initialize LED drivers and animators..."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), F("Removing old drivers and animators... "));

	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		if (ledDriver[i])
		{
			delete ledDriver[i];
		}
		if (ledAnimator[i])
		{
			delete ledAnimator[i];
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), F("Memory cleaned."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), F("Loading new drivers and animators... "));

	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		const TesLight::Configuration::LedConfig config = configuration->getLedConfig(i);

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Initialize LED driver for ") + config.ledCount + F(" LED's on pin ") + config.ledPin + F("."));

		ledDriver[i] = new TesLight::LedDriver(config.ledPin, i, config.ledCount);
		if (!ledDriver[i]->begin())
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("main.cpp:initializeLedDrivers"), (String)F("Failed to initialize LED driver for ") + config.ledCount + F(" LED's on pin ") + config.ledPin + F("."));
			return false;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Initialize LED driver for ") + config.ledCount + F(" LED's on pin ") + config.ledPin + F("."));
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Initialize LED animator of type ") + config.type + F(" for ") + config.ledCount + F(" LED's on pin ") + config.ledPin);

		// Rainbow solid type
		if (config.type == 0)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Type of animator ") + String(i) + F(" is rainbow solid."));
			ledAnimator[i] = new TesLight::RainbowAnimator();
			((TesLight::RainbowAnimator *)ledAnimator[i])->setRainbowMode(TesLight::RainbowMode::RAINBOW_SOLID);
		}

		// Rainbow linear type
		else if (config.type == 1)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Type of animator ") + String(i) + F(" is rainbow linear."));
			ledAnimator[i] = new TesLight::RainbowAnimator();
			((TesLight::RainbowAnimator *)ledAnimator[i])->setRainbowMode(TesLight::RainbowMode::RAINBOW_LINEAR);
		}

		// Rainbow middle type
		else if (config.type == 2)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Type of animator ") + String(i) + F(" is rainbow middle."));
			ledAnimator[i] = new TesLight::RainbowAnimator();
			((TesLight::RainbowAnimator *)ledAnimator[i])->setRainbowMode(TesLight::RainbowMode::RAINBOW_CENTER);
		}

		// Gradient type
		else if (config.type == 3)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Type of animator ") + String(i) + F(" is gradient."));
			ledAnimator[i] = new TesLight::GradientAnimator();
			((TesLight::GradientAnimator *)ledAnimator[i])->setColor(config.customField[0], config.customField[1], config.customField[2], config.customField[3], config.customField[4], config.customField[5]);
		}

		// Static color type
		else if (config.type == 4)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Type of animator ") + String(i) + F(" is static color."));
			ledAnimator[i] = new TesLight::StaticColorAnimator();
			((TesLight::StaticColorAnimator *)ledAnimator[i])->setColor(config.customField[0], config.customField[1], config.customField[2]);
		}

		ledAnimator[i]->setPixels(ledDriver[i]->getPixels());
		ledAnimator[i]->setPixelCount(ledDriver[i]->getPixelCount());
		ledAnimator[i]->setSpeed(config.speed);
		ledAnimator[i]->setOffset(config.offset);
		ledAnimator[i]->setBrightness(config.brightness / 255.0f);
		ledAnimator[i]->setReverse(config.reverse);
		ledAnimator[i]->init();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), F("LED drivers and animators initialized."));
	return true;
}

/**
 * @brief Initialize the switch sensor to turn on/off the LEDs together with the Tesla lights.
 */
void initializeSwitchSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeSwitchSensor"), F("Initializing switch sensor..."));
	switchSensor = new TesLight::SwitchSensor(SWITCH_SENSOR_PIN, SWITCH_SENSOR_THRESHOLD);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeSwitchSensor"), F("Switch sensor initialized."));
}

/**
 * @brief Initialize the motion sensor. In case the initialization fails the motion sensor pointer will be set to {@code nullptr}.
 * @return true when successful
 * @return false when there was an error
 */
bool initializeMotionSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeMotionSensor"), F("Initializing motion sensor..."));

	motionSensor = new TesLight::MotionSensor(MOTION_SENSOR_I2C_ADDRESS, SDA_PIN, SCL_PIN, MOTION_SENSOR_BUFFER_SIZE);
	if (motionSensor->begin())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeMotionSensor"), F("Motion sensor initialized."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:initializeMotionSensor"), F("Failed to initializing motion sensor."));
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeWiFiManager"), F("Initialize WiFi manager..."));

	wifiManager = new TesLight::WiFiManager();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeWiFiManager"), F("WiFi manager initialized."));
}

/**
 * @brief Create a WiFi access point and connect to the wifi network.
 * @return true when successful
 * @return false when there was an error
 */
bool createtWiFiNetwork()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:createtWiFiNetwork"), F("Creating WiFi network..."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:createtWiFiNetwork"), F("Starting WiFi access point..."));

	if (wifiManager->startAccessPoint(configuration->getWiFiConfig().accessPointSsid.c_str(), configuration->getWiFiConfig().accessPointPassword.c_str(), configuration->getWiFiConfig().accessPointChannel, false, configuration->getWiFiConfig().accessPointMaxConnections))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:createtWiFiNetwork"), F("WiFi AP started and running."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("main.cpp:createtWiFiNetwork"), F("Failed to start WiFi access point. Please check your configuration."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:createtWiFiNetwork"), F("WiFi network created."));
	return true;
}

/**
 * @brief Initialize the webserver to serve static files.
 */
void initializeWebServer()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeWebServer"), F("Initializing webserver..."));

	webServer = new TesLight::WebServer(WEB_SERVER_PORT, &SD, WEB_SERVER_STATIC_CONTENT);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeWebServer"), F("Webserver initialized."));
}

/**
 * @brief Initialize the REST api.
 */
void initializeRestApi()
{
	TesLight::ConnectionTestEndpoint::init(webServer, "/api/");
	TesLight::ConnectionTestEndpoint::begin();
	TesLight::WiFiConfigurationEndpoint::init(webServer, "/api/config/");
	TesLight::WiFiConfigurationEndpoint::begin(configuration, wifiConfigChanged);
	TesLight::LedConfigurationEndpoint::init(webServer, "/api/config/");
	TesLight::LedConfigurationEndpoint::begin(configuration, ledConfigChanged);
}

/**
 * @brief Stop the program.
 */
void stop()
{
	while (1)
		;
}

/**
 * @brief Initialize the software and hardware.
 */
void setup()
{
	initializeLogger(false);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Booting up..."));
	printLogo();

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize SD card..."));
	if (initializeSdCard())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("SD card initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to initialize SD card."));
		stop();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Switching to SD card logger..."));
	delay(500);
	if (initializeLogger(true))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Switched to SD card logger."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to switch to SD card logger."));
		stop();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize, load and save configuration..."));
	if (initializeConfiguration())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Runtime configuration loaded."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to initialize runtime configuration. Trying to continue with defaults."));
		configuration->loadDefaults();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize LED drivers and animator..."));
	if (initializeLedDrivers())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("LED drivers and animators initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to initialize LED drivers. Trying to continue without rendering LEDs."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize switch sensor..."));
	initializeSwitchSensor();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Switch sensor initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize motion sensor..."));
	if (initializeMotionSensor())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Motion sensor initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to initialize motion sensor. Trying to continue without motion sensor data."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize WiFiManager..."));
	initializeWiFiManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("WiFi manager initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Creating to WiFi network..."));
	if (createtWiFiNetwork())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("WiFi Network created."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to create WiFi network. Continuing without WiFi network. The REST API might be inaccessible."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Starting Webserver..."));
	initializeWebServer();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), (String)F("Webserver started on port ") + WEB_SERVER_PORT + F("."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initializing REST api..."));
	initializeRestApi();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), (String)F("REST api initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("TesLight initialized successfully, going into work mode."));
}

/**
 * @brief Main loop runs as long as the controller is active.
 */
void loop()
{
	// Handle the LEDs
	if (micros() - ledTimer > LED_CYCLE_TIME && ledTimer != MAX_ULONG_VALUE)
	{
		ledTimer = micros();

		for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
		{
			if (ledAnimator[i] == nullptr || ledDriver[i] == nullptr)
			{
				break;
			}

			ledAnimator[i]->render();
			if (!ledDriver[i]->show())
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:loop"), F("Failed to send out LED data because the driver returned with an error. LED's are disabled now."));
				ledTimer = MAX_ULONG_VALUE;
				break;
			}
		}

		ledFrameCounter++;
	}

	// Handle the switch sensor
	if (micros() - switchSensorTimer > SWITCH_SENSOR_CYCLE_TIME && switchSensorTimer != MAX_ULONG_VALUE)
	{
		switchSensorTimer = millis();

		if (switchSensor != nullptr)
		{
			const bool active = switchSensor->isActive();
			for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
			{
				if (ledDriver[i] != nullptr)
				{
					ledDriver[i]->setActive(active);
				}
			}
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:loop"), F("Failed to read switch sensor data. Switch sensor is disabled now."));
			switchSensorTimer = MAX_ULONG_VALUE;
		}
	}

	// Handle the motion sensor
	if (micros() - motionSensorTimer > MOTION_SENSOR_CYCLE_TIME && motionSensorTimer != MAX_ULONG_VALUE)
	{
		motionSensorTimer = micros();

		if (motionSensor == nullptr || !motionSensor->readData())
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:loop"), F("Failed to read motion sensor data. Motion sensor is disabled now."));
			motionSensorTimer = MAX_ULONG_VALUE;
		}
	}

	// Measure the FPS
	if (micros() - fpsTimer >= FPS_CYCLE_TIME)
	{
		fpsTimer = micros();

		ledFrameCounter /= FPS_CYCLE_TIME / 1000000;
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:loop"), (String)F("LEDs running at an average of ") + ledFrameCounter + F(" FPS"));

		ledFrameCounter = 0;
	}

	// Handle configuration updates when the flags are set
	if (wifiConfigChangedFlag)
	{
		wifiConfigChangedFlag = false;
		createtWiFiNetwork();
	}
	if (ledConfigChangedFlag)
	{
		ledConfigChangedFlag = false;
		initializeLedDrivers();
	}
}

/**
 * @brief Is called by the {@link TesLight::Configuration::WiFiConfigurationEndpoint} when the WiFi configuration was updated.
 * Since it's called async by the WebServer it should be used with care. Actual update should only happen in main loop.
 */
void wifiConfigChanged()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:wifiConfigChanged"), F("The WiFi configuration was updated, setting flag."));
	wifiConfigChangedFlag = true;
}

/**
 * @brief Is called by the {@link TesLight::Configuration::LedConfigurationEndpoint} when the LED configuration was updated.
 * Since it's called async by the WebServer it should be used with care. The actual update should only happen in the main loop.
 */
void ledConfigChanged()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:ledConfigChanged"), F("The LED configuration was updated, setting flag."));
	ledConfigChangedFlag = true;
}
