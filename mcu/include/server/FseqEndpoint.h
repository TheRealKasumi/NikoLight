/**
 * @file FseqEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to manage fseq files on the TesLight controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef FSEQ_ENDPOINT_H
#define FSEQ_ENDPOINT_H

#include <FS.h>

#include "server/RestEndpoint.h"
#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "logging/Logger.h"
#include "util/FileUtil.h"
#include "util/FseqLoader.h"

namespace TesLight
{
	class FseqEndpoint : public RestEndpoint
	{
	public:
		static void begin(FS *_fileSystem, TesLight::Configuration *_configuration);

	private:
		FseqEndpoint();

		static FS *fileSystem;
		static TesLight::Configuration *configuration;
		static File uploadFile;

		static void getFseqList();
		static void postFseq();
		static void fseqUpload();
		static void deleteFseq();

		static bool verifyFileName(const String fileName);
		static bool verifyFseqFile(const String fileName);
	};
}

#endif
