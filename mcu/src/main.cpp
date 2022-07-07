/**
 * @file main.cpp
 * @author TheRealKasumi
 * @brief Entry point, initialization and main loop of the program.
 * @version 0.0.1
 * @date 2022-06-22
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
#include "motion/MotionSensor.h"
#include "wifi/WiFiManager.h"
#include "server/WebServer.h"

#include "led/animators/RainbowAnimator.h"
#include "led/animators/GradientAnimator.h"
#include "led/animators/StaticColorAnimator.h"

TesLight::Configuration *configuration = nullptr;
TesLight::LedDriver *ledDriver[NUM_LED_DRIVERS] = {nullptr};
TesLight::LedAnimator *ledAnimator[NUM_LED_DRIVERS] = {nullptr};
TesLight::MotionSensor *motionSensor = nullptr;
TesLight::WiFiManager *wifiManager = nullptr;
TesLight::WebServer *webServer = nullptr;

uint64_t ledTimer = 0;
uint64_t motionSensorTimer = 0;
uint64_t fpsTimer = 0;

uint16_t ledFrameCounter = 0;

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
bool initializedConfiguration()
{
	configuration = new TesLight::Configuration(&SD, CONFIGURATION_FILE_NAME);
	return configuration->load();
}

/**
 * @brief Initialize new led drivers and deletes old ones.
 * @return true when initialized successfully
 * @return false when error
 */
bool initializeLedDrivers()
{
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		if (ledDriver[i])
		{
			delete ledDriver[i];
		}

		const TesLight::LedDriverConfig config = configuration->getLedDriverConfig(i);
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedDrivers"), (String)F("Initialize LED driver for ") + config.ledCount + F(" LED's on pin ") + config.ledPin);

		ledDriver[i] = new TesLight::LedDriver(config.ledPin, i, config.ledCount);
		if (!ledDriver[i]->begin())
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Initialize new led animators and deletes old ones.
 * @return true when initialized successfully
 * @return false when error
 */
bool initializeLedAnimators()
{
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		if (ledAnimator[i])
		{
			delete ledAnimator[i];
		}

		const TesLight::LedDriverConfig driverConfig = configuration->getLedDriverConfig(i);
		const TesLight::LedAnimatorConfig animatorConfig = configuration->getLedAnimatorConfig(i);
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("main.cpp:initializeLedAnimators"), (String)F("Initialize LED animator of type ") + animatorConfig.type + F(" for ") + driverConfig.ledCount + F(" LED's on pin ") + driverConfig.ledPin);

		// Rainbow type
		if (animatorConfig.type == 0)
		{
			ledAnimator[i] = new TesLight::RainbowAnimator();
			((TesLight::RainbowAnimator *)ledAnimator[i])->setRainbowMode(animatorConfig.customField[0] == 0 ? TesLight::RainbowMode::RAINBOW_LINEAR : TesLight::RainbowMode::RAINBOW_CENTER);
		}

		// Gradient type
		else if (animatorConfig.type == 1)
		{
			ledAnimator[i] = new TesLight::GradientAnimator();
			((TesLight::GradientAnimator *)ledAnimator[i])->setColor(animatorConfig.customField[0], animatorConfig.customField[1], animatorConfig.customField[2], animatorConfig.customField[3], animatorConfig.customField[4], animatorConfig.customField[5]);
		}

		// Static color type
		else if (animatorConfig.type == 1)
		{
			ledAnimator[i] = new TesLight::StaticColorAnimator();
			((TesLight::StaticColorAnimator *)ledAnimator[i])->setColor(animatorConfig.customField[0], animatorConfig.customField[1], animatorConfig.customField[2]);
		}

		ledAnimator[i]->setPixels(ledDriver[i]->getPixels());
		ledAnimator[i]->setPixelCount(ledDriver[i]->getPixelCount());
		ledAnimator[i]->setSpeed(animatorConfig.speed);
		ledAnimator[i]->setOffset(animatorConfig.offset);
		ledAnimator[i]->setBrightness(animatorConfig.brightness / 255.0f);
		ledAnimator[i]->setReverse(animatorConfig.reverse);
		ledAnimator[i]->init();
	}

	return true;
}

/**
 * @brief Initialize the motion sensor.
 * @return true when successful
 * @return false when there was an error
 */
bool initializeMotionSensor()
{
	motionSensor = new TesLight::MotionSensor(MOTION_SENSOR_I2C_ADDRESS, SDA_PIN, SCL_PIN, MOTION_SENSOR_BUFFER_SIZE);
	return motionSensor->begin();
}

/**
 * @brief Initialize the wifi manager.
 */
void initializeWiFiManager()
{
	wifiManager = new TesLight::WiFiManager();
}

/**
 * @brief Initialize the webserver to serve static files and handle rest requests.
 */
void initializeWebServer()
{
	webServer = new TesLight::WebServer(WEB_SERVER_PORT, &SD, WEB_SERVER_STATIC_CONTENT);
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize and load configuration..."));
	if (initializedConfiguration())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Runtime configuration loaded."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to load runtime configuration."));
		stop();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize LED drivers..."));
	if (initializeLedDrivers())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("LED drivers initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to initialize LED drivers."));
		stop();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize LED animators..."));
	if (initializeLedAnimators())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("LED animators initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to initialize LED animators."));
		stop();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize motion sensor..."));
	if (initializeMotionSensor())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Motion sensor initialized."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to initialize motion sensor."));
		stop();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Initialize WiFiManager..."));
	initializeWiFiManager();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("WiFi manager initialized."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Starting WiFi access point..."));
	if (wifiManager->startAccessPoint(configuration->getApSSID().c_str(), configuration->getApPassword().c_str(), configuration->getApChannel(), false, configuration->getApMaxConnections()))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("WiFi AP started and running."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:setup"), F("Failed to start WiFi access point. Please check your configuration."));
		stop();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Starting Webserver..."));
	initializeWebServer();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), (String)F("Webserver started on port ") + WEB_SERVER_PORT + F("."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:setup"), F("Program initialized successfully, going into work mode."));
}

/**
 * @brief Main loop runs as long as the controller is active.
 */
void loop()
{
	// Handle the LEDs
	if (millis() - ledTimer > LED_CYCLE_TIME)
	{
		ledTimer = millis();

		for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
		{
			ledAnimator[i]->render();
			if (!ledDriver[i]->show())
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:loop"), F("Failed to send out LED data because the driver returned with an error."));
				ledTimer = 0xffffffffffffffff;
				break;
			}
		}

		ledFrameCounter++;
	}

	// Handle the motion sensor
	if (millis() - motionSensorTimer > MOTION_SENSOR_CYCLE_TIME)
	{
		motionSensorTimer = millis();

		if (!motionSensor->readData())
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("main.cpp:loop"), F("Failed to read motion sensor data."));
			motionSensorTimer = 0xffffffffffffffff;
		}
	}

	// Measure the FPS
	if (millis() - fpsTimer >= FPS_CYCLE_TIME)
	{
		fpsTimer = millis();

		ledFrameCounter /= FPS_CYCLE_TIME / 1000;
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("main.cpp:loop"), (String)F("LEDs running at an average of ") + ledFrameCounter + F(" FPS"));

		ledFrameCounter = 0;
	}
}
