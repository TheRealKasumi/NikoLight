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

#include "configuration/SystemConfiguration.h"
#include "server/RestEndpoint.h"
#include "logging/Logger.h"
#include "util/FileUtil.h"

namespace TesLight
{
	class FseqEndpoint : public RestEndpoint
	{
	public:
		static void begin(FS *_fileSystem);

	private:
		FseqEndpoint();

		static FS *fileSystem;
		static File uploadFile;

		static void getFseqList(AsyncWebServerRequest *request);
		static void postFseq(AsyncWebServerRequest *request);
		static void postFseqBody(AsyncWebServerRequest *request, uint8_t *data, const size_t len, const size_t index, const size_t total);
		static void deleteFseq(AsyncWebServerRequest *request);
	};
}

#endif
