/**
 * @file WatchDog.h
 * @author TheRealKasumi
 * @brief Class to handle the watchdog timers.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef WATCH_DOG_H
#define WATCH_DOG_H

#include <esp_task_wdt.h>
#include "configuration/SystemConfiguration.h"

namespace TL
{
	class WatchDog
	{
	public:
		static bool initializeTaskWatchdog();
		static bool resetTaskWatchdog();
		static bool deleteTaskWatchdog();

	private:
		WatchDog();
		~WatchDog();
	};
}

#endif
