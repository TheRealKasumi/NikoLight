/**
 * @file FileUtil.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::WatchDog}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "util/WatchDog.h"

/**
 * @brief Initialize the task watchdog timer.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::WatchDog::initializeTaskWatchdog()
{
	return esp_task_wdt_init(WATCHDOG_RESET_TIME, true) == ESP_OK && esp_task_wdt_add(NULL) == ESP_OK;
}

/**
 * @brief Reset the task watchdog timer.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::WatchDog::resetTaskWatchdog()
{
	return esp_task_wdt_reset() == ESP_OK;
}

/**
 * @brief Delete the task watchdog timer.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::WatchDog::deleteTaskWatchdog()
{
	return esp_task_wdt_reset() == ESP_OK && esp_task_wdt_delete(NULL) == ESP_OK;
}
