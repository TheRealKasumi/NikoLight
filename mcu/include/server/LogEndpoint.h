/**
 * @file LogEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to manage the log file of the NikoLight controller.
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
#ifndef LOG_ENDPOINT_H
#define LOG_ENDPOINT_H

#include <FS.h>
#include "configuration/SystemConfiguration.h"
#include "server/RestEndpoint.h"
#include "logging/Logger.h"

namespace NL
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
