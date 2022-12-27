/**
 * @file WiFiManager.h
 * @author TheRealKasumi
 * @brief Contains a simple WiFi manager.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

#include "logging/Logger.h"

namespace TL
{
	class WiFiManager
	{
	public:
		WiFiManager();
		~WiFiManager();

		bool startAccessPoint(const char *ssid, const char *password = 0, uint8_t channel = 1, bool hidden = 0, uint8_t maxConnections = 1);
		bool connectTo(const char *ssid, const char *password = 0, const uint32_t timeout = 5000);
	};
}

#endif
