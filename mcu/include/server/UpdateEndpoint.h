/**
 * @file UpdateEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to load update packages to the TesLight controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef UPDATE_ENDPOINT_H
#define UPDATE_ENDPOINT_H

#include <FS.h>

#include "configuration/SystemConfiguration.h"
#include "server/RestEndpoint.h"
#include "logging/Logger.h"
#include "util/FileUtil.h"
#include "update/Updater.h"

namespace TesLight
{
	class UpdateEndpoint : public RestEndpoint
	{
	public:
		static void begin(FS *_fileSystem);

	private:
		UpdateEndpoint();

		static FS *fileSystem;
		static File uploadFile;

		static void postPackage();
		static void packageUpload();
	};
}

#endif
