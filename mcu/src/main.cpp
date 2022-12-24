/**
 * @file main.cpp
 * @author TheRealKasumi
 * @brief Entry point, initialization and main loop of the program.
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "TesLight.h"

/**
 * @brief Initialize the software and hardware.
 */
void setup()
{
	TesLight::begin();
}

/**
 * @brief Main loop runs as long as the controller is active.
 */
void loop()
{
	TesLight::run();
}