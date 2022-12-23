/**
 * @file TesLight.h
 * @author TheRealKasumi
 * @brief Static class encapsulating all TesLight functionality.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef TES_LIGHT_MAIN_H
#define TES_LIGHT_MAIN_H

#include <Arduino.h>
#include <memory>
#include <SD.h>

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
#include "util/WatchDog.h"
#include "update/Updater.h"

class TesLight
{
public:
	static void begin();
	static void run();

private:
	TesLight();
	~TesLight();

	// References to dynamically allocated instances
	static std::unique_ptr<TL::Configuration> configuration;
	static std::unique_ptr<TL::FanController> fanController;
	static std::unique_ptr<TL::LedManager> ledManager;
	static std::unique_ptr<TL::TemperatureSensor> temperatureSensor;
	static std::unique_ptr<TL::LightSensor> lightSensor;
	static std::unique_ptr<TL::MotionSensor> motionSensor;
	static std::unique_ptr<TL::WiFiManager> wifiManager;
	static std::unique_ptr<TL::WebServerManager> webServerManager;

	// Timer and counter
	static unsigned long ledInterval;
	static unsigned long lightSensorInterval;
	static unsigned long motionSensorInterval;
	static unsigned long webServerInterval;
	static unsigned long statusInterval;
	static unsigned long temperatureInterval;
	static unsigned long ledTimer;
	static unsigned long lightSensorTimer;
	static unsigned long motionSensorTimer;
	static unsigned long webServerTimer;
	static unsigned long statusTimer;
	static unsigned long temperatureTimer;
	static uint16_t ledFrameCounter;
	static float ledPowerCounter;

	// Initialization functions
	static void printLogo();
	static bool initializeLogger(bool sdLogging);
	static bool initializeSdCard();
	static bool initializeConfiguration();
	static void intializeI2C();
	static void initializeFanController();
	static void initializeLedManager();
	static void initializeTemperatureSensor();
	static void initializeLightSensor();
	static bool initializeMotionSensor();
	static void initializeWiFiManager();
	static void initializeWebServerManager();
	static void initializeRestApi();
	static void initializeTimers();
	static bool checkTimer(unsigned long &timer, unsigned long cycleTime, bool &skipFrame);

	// System update functions
	static bool updateAvilable();
	static void handleUpdate();

	// WiFi functions
	static bool createtWiFiNetwork();

	// Callback functions for the rest endpoints
	static bool applySystemConfig();
	static bool applyLedConfig();
	static bool applyWifiConfig();
};

#endif
