/**
 * @file ResetEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to remotely reset the controller in case something is going wrong.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RESET_ENDPOINT_H
#define RESET_ENDPOINT_H

#include "configuration/SystemConfiguration.h"
#include "server/RestEndpoint.h"
#include "logging/Logger.h"
#include "update/Updater.h"

namespace TesLight
{
	class ResetEndpoint : public RestEndpoint
	{
	public:
		static void begin(FS *_fileSystem);

	private:
		ResetEndpoint();

		static FS *fileSystem;

		static void handleSoftReset();
		static void handleHardReset();
	};
}

#endif
