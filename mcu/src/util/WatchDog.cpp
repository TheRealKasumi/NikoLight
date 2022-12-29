/**
 * @file FileUtil.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::WatchDog}.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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