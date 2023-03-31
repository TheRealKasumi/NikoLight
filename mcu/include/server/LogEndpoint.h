/**
 * @file LogEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to manage the log file of the TesLight controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LOG_ENDPOINT_H
#define LOG_ENDPOINT_H

#include <FS.h>
#include <esp_task_wdt.h>

#include "configuration/SystemConfiguration.h"
#include "server/RestEndpoint.h"
#include "logging/Logger.h"

namespace TesLight
{
	class LogEndpoint : public RestEndpoint
	{
	public:
		static void begin(FS *_fileSystem);

	private:
		LogEndpoint();

		static FS *fileSystem;

		static void getLogSize(AsyncWebServerRequest *request);
		static void getLog(AsyncWebServerRequest *request);
		static void clearLog(AsyncWebServerRequest *request);
	};
}

#endif
