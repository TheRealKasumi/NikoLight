/**
 * @file WatchDog.h
 * @author TheRealKasumi
 * @brief Class to handle the watchdog timers.
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
