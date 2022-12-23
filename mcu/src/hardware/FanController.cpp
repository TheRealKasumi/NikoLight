/**
 * @file FanController.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::FanController}.
 *
 * @copyright Copyright (c) 2022
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
