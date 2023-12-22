/**
 * @file NikoLight.h
 * @author TheRealKasumi
 * @brief Static class encapsulating all NikoLight functionality.
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
#ifndef NIKO_LIGHT_MAIN_H
#define NIKO_LIGHT_MAIN_H

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
#include "hardware/Fan.h"
#include "hardware/LM75BD.h"
#include "led/LedManager.h"
#include "sensor/TemperatureSensor.h"
#include "sensor/LightSensor.h"
#include "sensor/MotionSensor.h"
#include "wifi/WiFiManager.h"
#include "server/WebServerManager.h"
#include "server/ConnectionTestEndpoint.h"
#include "server/ProfileEndpoint.h"
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

class NikoLight
{
public:
	static void begin();
	static void run();

private:
	NikoLight();

	// Timer
	static unsigned long lightSensorInterval;
	static unsigned long motionSensorInterval;
	static unsigned long audioUnitInterval;
	static unsigned long frameTimer;
	static unsigned long lightSensorTimer;
	static unsigned long motionSensorTimer;
	static unsigned long audioUnitTimer;
	static unsigned long temperatureTimer;
	static unsigned long statusTimer;
	static unsigned long statusPrintTimer;
	static unsigned long webServerTimer;

	// Counter
	static uint16_t frameCounter;
	static float ledPowerCounter;

	// Workaround for v2.2
	#if defined(HW_VERSION_2_2)
		static NL::LM75BD *lm75bd;
	#endif

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
	static void initializeFan();
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
