/**
 * @file WatchDog.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::WatchDog}.
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
#include "util/WatchDog.h"

/**
 * @brief Initialize the task watchdog timer.
 * @return OK when the watchdog was initialized
 * @return ERROR_OUT_OF_MEMORY when there was no memory for the allocation
 * @return ERROR_ALREADY_SUBSCRIBED when the task alreadyd subscribed to a wdt
 * @return ERROR_WDT_NOT_INIT when the wdt was not initialized beforehand
 * @return ERROR_UNKNOWN when a unknown error occurs
 */
NL::WatchDog::Error NL::WatchDog::initializeTaskWatchdog()
{
	const esp_err_t initError = esp_task_wdt_init(WATCHDOG_RESET_TIME, true);
	if (initError == ESP_ERR_NO_MEM)
	{
		return NL::WatchDog::Error::ERROR_OUT_OF_MEMORY;
	}
	else if (initError != ESP_OK)
	{
		return NL::WatchDog::Error::ERROR_UNKNOWN;
	}

	const esp_err_t addError = esp_task_wdt_add(NULL);
	if (addError == ESP_ERR_INVALID_ARG)
	{
		return NL::WatchDog::Error::ERROR_ALREADY_SUBSCRIBED;
	}
	else if (addError == ESP_ERR_INVALID_STATE)
	{
		return NL::WatchDog::Error::ERROR_WDT_NOT_INIT;
	}
	else if (addError != ESP_OK)
	{
		return NL::WatchDog::Error::ERROR_UNKNOWN;
	}

	return NL::WatchDog::Error::OK;
}

/**
 * @brief Reset the task watchdog timer.
 * @return OK when the watchdog was reset
 * @return ERROR_WDT_NOT_FOUND when no watchdog was subscribed by the current task
 * @return ERROR_WDT_NOT_INIT when the wdt was not initialized beforehand
 * @return ERROR_UNKNOWN when a unknown error occurs
 */
NL::WatchDog::Error NL::WatchDog::resetTaskWatchdog()
{
	const esp_err_t resetError = esp_task_wdt_reset();
	if (resetError == ESP_ERR_NOT_FOUND)
	{
		return NL::WatchDog::Error::ERROR_WDT_NOT_FOUND;
	}
	else if (resetError == ESP_ERR_INVALID_STATE)
	{
		return NL::WatchDog::Error::ERROR_WDT_NOT_INIT;
	}
	else if (resetError != ESP_OK)
	{
		return NL::WatchDog::Error::ERROR_UNKNOWN;
	}

	return NL::WatchDog::Error::OK;
}

/**
 * @brief Delete the task watchdog timer.
 * @return OK when the wdt was deleted
 * @return ERROR_WDT_NOT_FOUND when no watchdog was subscribed by the current task
 * @return ERROR_WDT_NOT_INIT when the wdt was not initialized beforehand
 * @return ERROR_UNKNOWN when a unknown error occurs
 */
NL::WatchDog::Error NL::WatchDog::deleteTaskWatchdog()
{
	const NL::WatchDog::Error resetError = NL::WatchDog::resetTaskWatchdog();
	if (resetError != NL::WatchDog::Error::OK)
	{
		return resetError;
	}

	const esp_err_t deleteError = esp_task_wdt_delete(NULL);
	if (deleteError == ESP_ERR_INVALID_ARG)
	{
		return NL::WatchDog::Error::ERROR_WDT_NOT_FOUND;
	}
	else if (deleteError == ESP_ERR_INVALID_STATE)
	{
		return NL::WatchDog::Error::ERROR_WDT_NOT_INIT;
	}
	else if (deleteError != ESP_OK)
	{
		return NL::WatchDog::Error::ERROR_UNKNOWN;
	}

	return NL::WatchDog::Error::OK;
}
