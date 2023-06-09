/**
 * @file WatchDog.h
 * @author TheRealKasumi
 * @brief Class to handle the watchdog timers.
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
#ifndef WATCH_DOG_H
#define WATCH_DOG_H

#include <esp_task_wdt.h>
#include "configuration/SystemConfiguration.h"

namespace NL
{
	class WatchDog
	{
	public:
		enum class Error
		{
			OK,						  // No error
			ERROR_UNKNOWN,			  // Unknown error
			ERROR_OUT_OF_MEMORY,	  // Not enough memory for the allocation
			ERROR_ALREADY_SUBSCRIBED, // The task is already subscribed
			ERROR_WDT_NOT_INIT,		  // The wdt has not been initialized yet
			ERROR_WDT_NOT_FOUND,	  // The task did not subscribe to a wdt
		};

		static NL::WatchDog::Error initializeTaskWatchdog();
		static NL::WatchDog::Error resetTaskWatchdog();
		static NL::WatchDog::Error deleteTaskWatchdog();

	private:
		WatchDog();
		~WatchDog();
	};
}

#endif
