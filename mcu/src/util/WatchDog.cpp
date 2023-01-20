/**
 * @file FileUtil.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::WatchDog}.
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
#include "util/WatchDog.h"

/**
 * @brief Initialize the task watchdog timer.
 * @return OK when the watchdog was initialized
 * @return ERROR_OUT_OF_MEMORY when there was no memory for the allocation
 * @return ERROR_ALREADY_SUBSCRIBED when the task alreadyd subscribed to a wdt
 * @return ERROR_WDT_NOT_INIT when the wdt was not initialized beforehand
 *  * @return ERROR_UNKNOWN when a unknown error occurs
 */
TL::WatchDog::Error TL::WatchDog::initializeTaskWatchdog()
{
	const esp_err_t initError = esp_task_wdt_init(WATCHDOG_RESET_TIME, true);
	if (initError == ESP_ERR_NO_MEM)
	{
		return TL::WatchDog::Error::ERROR_OUT_OF_MEMORY;
	}
	else if (initError != ESP_OK)
	{
		return TL::WatchDog::Error::ERROR_UNKNOWN;
	}

	const esp_err_t addError = esp_task_wdt_add(NULL);
	if (addError == ESP_ERR_INVALID_ARG)
	{
		return TL::WatchDog::Error::ERROR_ALREADY_SUBSCRIBED;
	}
	else if (addError == ESP_ERR_INVALID_STATE)
	{
		return TL::WatchDog::Error::ERROR_WDT_NOT_INIT;
	}
	else if (addError != ESP_OK)
	{
		return TL::WatchDog::Error::ERROR_UNKNOWN;
	}

	return TL::WatchDog::Error::OK;
}

/**
 * @brief Reset the task watchdog timer.
 * @return OK when the watchdog was reset
 * @return ERROR_WDT_NOT_FOUND when no watchdog was subscribed by the current task
 * @return ERROR_WDT_NOT_INIT when the wdt was not initialized beforehand
 * @return ERROR_UNKNOWN when a unknown error occurs
 */
TL::WatchDog::Error TL::WatchDog::resetTaskWatchdog()
{
	const esp_err_t resetError = esp_task_wdt_reset();
	if (resetError == ESP_ERR_NOT_FOUND)
	{
		return TL::WatchDog::Error::ERROR_WDT_NOT_FOUND;
	}
	else if (resetError == ESP_ERR_INVALID_STATE)
	{
		return TL::WatchDog::Error::ERROR_WDT_NOT_INIT;
	}
	else if (resetError != ESP_OK)
	{
		return TL::WatchDog::Error::ERROR_UNKNOWN;
	}

	return TL::WatchDog::Error::OK;
}

/**
 * @brief Delete the task watchdog timer.
 * @return OK when the wdt was deleted
 * @return ERROR_WDT_NOT_FOUND when no watchdog was subscribed by the current task
 * @return ERROR_WDT_NOT_INIT when the wdt was not initialized beforehand
 * @return ERROR_UNKNOWN when a unknown error occurs
 */
TL::WatchDog::Error TL::WatchDog::deleteTaskWatchdog()
{
	const TL::WatchDog::Error resetError = TL::WatchDog::resetTaskWatchdog();
	if (resetError != TL::WatchDog::Error::OK)
	{
		return resetError;
	}

	const esp_err_t deleteError = esp_task_wdt_delete(NULL);
	if (deleteError == ESP_ERR_INVALID_ARG)
	{
		return TL::WatchDog::Error::ERROR_WDT_NOT_FOUND;
	}
	else if (deleteError == ESP_ERR_INVALID_STATE)
	{
		return TL::WatchDog::Error::ERROR_WDT_NOT_INIT;
	}
	else if (deleteError != ESP_OK)
	{
		return TL::WatchDog::Error::ERROR_UNKNOWN;
	}

	return TL::WatchDog::Error::OK;
}
