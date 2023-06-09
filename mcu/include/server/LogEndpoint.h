/**
 * @file LogEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to manage the log file of the NikoLight controller.
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
