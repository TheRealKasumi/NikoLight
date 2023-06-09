/**
 * @file ResetEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to remotely reset the controller in case something is going wrong.
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
#ifndef RESET_ENDPOINT_H
#define RESET_ENDPOINT_H

#include "configuration/SystemConfiguration.h"
#include "server/RestEndpoint.h"
#include "logging/Logger.h"
#include "update/Updater.h"

namespace NL
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
