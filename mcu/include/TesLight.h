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
#ifndef TES_LIGHT_MAIN_H
#define TES_LIGHT_MAIN_H

#include <Arduino.h>
#include <SD.h>

#include "SystemInformation.h"
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"
#include "configuration/Configuration.h"
#include "hardware/AnalogInput.h"
#include "hardware/MPU6050.h"
#include "hardware/DS18B20.h"
#include "hardware/BH1750.h"
#include "hardware/AudioUnit.h"
#include "hardware/FanController.h"
#include "led/LedManager.h"
#include "sensor/TemperatureSensor.h"
#include "sensor/LightSensor.h"
#include "sensor/MotionSensor.h"
#include "wifi/WiFiManager.h"
#include "server/WebServerManager.h"
#include "server/ConnectionTestEndpoint.h"
#include "server/SystemInformationEndpoint.h"
#include "server/SystemConfigurationEndpoint.h"
#include "server/LedConfigurationEndpoint.h"
#include "server/WiFiConfigurationEndpoint.h"
#include "server/FseqEndpoint.h"
#include "server/LogEndpoint.h"
#include "server/UpdateEndpoint.h"
#include "server/ResetEndpoint.h"
#include "server/MotionSensorEndpoint.h"
#include "server/AudioUnitConfigurationEndpoint.h"
#include "server/UIConfigurationEndpoint.h"
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

	// Timer
	static unsigned long lightSensorInterval;
	static unsigned long motionSensorInterval;
	static unsigned long audioUnitInterval;
	static unsigned long renderTimer;
	static unsigned long frameTimer;
	static unsigned long lightSensorTimer;
	static unsigned long motionSensorTimer;
	static unsigned long audioUnitTimer;
	static unsigned long temperatureTimer;
	static unsigned long statusTimer;
	static unsigned long statusPrintTimer;
	static unsigned long webServerTimer;

	// Counter
	static uint16_t renderCounter;
	static uint16_t frameCounter;
	static float ledPowerCounter;

	// Initialization functions
	static void printLogo();
	static void initializeLogger(const bool sdLogging);
	static void initializeSdCard();
	static void initializeSystemInformation();
	static void initializeConfiguration();
	static void initializeHardwareModules();
	static void initializeLedManager();
	static void initializeMotionSensor();
	static void initializeLightSensor();
	static void initializeTemperatureSensor();
	static void initializeFanController();
	static void initializeWiFiManager();
	static void initializeWebServerManager();
	static void initializeRestApi();
	static void initializeTimers();

	// System update functions
	static void handleUpdate();

	// WiFi functions
	static void createtWiFiNetwork();

	// Timer functions
	static bool checkTimer(unsigned long &timer, unsigned long cycleTime);
};

#endif
