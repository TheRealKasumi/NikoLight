/**
 * @file FanController.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::FanController}.
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
#include "hardware/FanController.h"

/**
 * @brief Create a new instance of {@link TL::FanController} and initialize the hardware.
 * @param fanPin output pin for the pwm signal to controll the fan
 * @param configuration reference to the configuration of the controller
 */
TL::FanController::FanController(const uint8_t fanPin, TL::Configuration *configuration)
{
	this->fanPin = fanPin;
	this->configuration = configuration;
	ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	ledcAttachPin(this->fanPin, FAN_PWM_CHANNEL);
}

/**
 * @brief Destroy the {@link TL::FanController} instance and free resources.
 */
TL::FanController::~FanController()
{
	ledcDetachPin(this->fanPin);
}

/**
 * @brief Set the temperature to calculate the new fan speed.
 * @param temp current temperature
 */
void TL::FanController::setTemperature(const uint8_t temp)
{
	const TL::Configuration::SystemConfig systemConfig = this->configuration->getSystemConfig();

	// Convert the temperature into a value between 0 and 1 within the min/max limits
	float temperature = temp;
	if (temperature < systemConfig.fanMinTemperature)
	{
		temperature = systemConfig.fanMinTemperature;
	}
	else if (temperature > systemConfig.fanMaxTemperature)
	{
		temperature = systemConfig.fanMaxTemperature;
	}
	temperature = (temperature - systemConfig.fanMinTemperature) / (systemConfig.fanMaxTemperature - systemConfig.fanMinTemperature);

	// Based on the temperature value, calculate the pwm value
	uint16_t pwm = temperature == 0.0f ? 0.0f : (systemConfig.fanMaxPwmValue - systemConfig.fanMinPwmValue) * temperature + systemConfig.fanMinPwmValue;
	if (pwm > systemConfig.fanMaxPwmValue)
	{
		pwm = systemConfig.fanMaxPwmValue;
	}

	ledcWrite(FAN_PWM_CHANNEL, pwm);
}
