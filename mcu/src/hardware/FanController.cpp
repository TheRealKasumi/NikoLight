/**
 * @file FanController.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::FanController}.
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
#include "hardware/FanController.h"

bool TL::FanController::initialized = false;
uint8_t TL::FanController::fanPin;
uint8_t TL::FanController::pwmChannel;
uint32_t TL::FanController::frequency;
uint8_t TL::FanController::resolution;

/**
 * @brief Initialize the {@link TL::FanController}. This required the configuration to be initialized beforehand.
 * @param fanPin output pin for the pwm signal to controll the fan
 * @param pwmChannel channel for the PWM output
 * @param frequency frequency of the pwm output
 * @param resolution resolution of the pwm output
 * @return OK when the fan controller was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration is not available
 * @return ERROR_SETUP_PIN when the output pin could not be configured
 */
TL::FanController::Error TL::FanController::begin(const uint8_t fanPin, const uint8_t pwmChannel, const uint32_t frequency, const uint8_t resolution)
{
	TL::FanController::initialized = false;
	TL::FanController::fanPin = fanPin;
	TL::FanController::pwmChannel = pwmChannel;
	TL::FanController::frequency = frequency;
	TL::FanController::resolution = resolution;

	if (!TL::Configuration::isInitialized())
	{
		return TL::FanController::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	if (ledcSetup(TL::FanController::pwmChannel, TL::FanController::frequency, TL::FanController::resolution) == 0)
	{
		return TL::FanController::Error::ERROR_SETUP_PIN;
	}
	ledcAttachPin(TL::FanController::fanPin, TL::FanController::pwmChannel);

	TL::FanController::initialized = true;
	return TL::FanController::Error::OK;
}

/**
 * @brief Stop the fan controller.
 */
void TL::FanController::end()
{
	TL::FanController::initialized = false;
	ledcDetachPin(TL::FanController::fanPin);
}

/**
 * @brief Return if the controller was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::FanController::isInitialized()
{
	return TL::FanController::initialized;
}

/**
 * @brief Set the temperature to calculate the new fan speed.
 * @param temp current temperature
 */
void TL::FanController::setTemperature(const uint8_t temp)
{
	const TL::Configuration::SystemConfig systemConfig = TL::Configuration::getSystemConfig();

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

	ledcWrite(TL::FanController::pwmChannel, pwm);
}
