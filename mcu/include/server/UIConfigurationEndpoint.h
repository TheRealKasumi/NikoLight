/**
 * @file UIConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to read and save UI related settings.
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
#ifndef UI_CONFIGURATION_ENDPOINT_H
#define UI_CONFIGURATION_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "logging/Logger.h"

namespace NL
{
	class UIConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin();

	private:
		UIConfigurationEndpoint();

		static void getUIConfig();
		static void patchUIConfig();

		static bool validateUIConfig(const JsonObject &jsonObject);
	};
}

#endif
