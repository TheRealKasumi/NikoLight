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
#include "configuration/SystemConfiguration.h"
#include "server/RestEndpoint.h"
#include "logging/Logger.h"

namespace TL
{
	class LogEndpoint : public RestEndpoint
	{
	public:
		static void begin(FS *_fileSystem);

	private:
		LogEndpoint();

		static FS *fileSystem;

		static void getLogSize();
		static void getLog();
		static void clearLog();
	};
}

#endif
