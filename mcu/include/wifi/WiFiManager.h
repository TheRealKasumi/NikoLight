/**
 * @file WiFiManager.h
 * @author TheRealKasumi
 * @brief Contains a simple WiFi manager.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "logging/Logger.h"

namespace TesLight
{
	class WiFiManager
	{
	public:
		WiFiManager(TesLight::Logger *logger);
		~WiFiManager();

		bool startAccessPoint(const char *ssid, const char *password = 0, uint8_t channel = 1, bool hidden = 0, uint8_t maxConnections = 1);
		bool connectTo(const char *ssid, const char *password = 0, const uint32_t timeout = 5000);

	private:
		TesLight::Logger *logger;

		void init();
	};
}

#endif
