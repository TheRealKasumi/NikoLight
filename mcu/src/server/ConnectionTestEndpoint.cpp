/**
 * @file ConnectionTestEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to test the connection to the controller.
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
#include "server/ConnectionTestEndpoint.h"

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::ConnectionTestEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("connection_test")).c_str(), http_method::HTTP_GET, NL::ConnectionTestEndpoint::handleConnectionTest);
}

/**
 * @brief Handler function for the connection test.
 */
void NL::ConnectionTestEndpoint::handleConnectionTest()
{
	NL::ConnectionTestEndpoint::sendSimpleResponse(200, F("I am doing alright, thanks for asking :3 !"));
}