/**
 * @file FanController.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::FanController}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "hardware/FanController.h"

/**
 * @brief Create a new instance of {@link TesLight::FanController} and initialize the hardware.
 * @param fanPin output pin for the pwm signal to controll the fan
 * @param pwmMin minimum pwm value at which the output will start (stall protection)
 * @param pwmMax maximum pwm value that will be output
 * @param tempMin minimum temperature at which the fan will start
 * @param tempMax maximum temperature at which the fan will run at max speed
 */
TesLight::FanController::FanController(const uint8_t fanPin, const uint16_t pwmMin, const uint16_t pwmMax, const uint8_t tempMin, const uint8_t tempMax)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initialize fan controller for pin ") + String(this->fanPin) + F("."));
	this->fanPin = fanPin;
	this->pwmMin = pwmMin;
	this->pwmMax = pwmMax;
	this->tempMin = tempMin;
	this->tempMax = tempMax;
	ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
	ledcAttachPin(this->fanPin, FAN_PWM_CHANNEL);
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Fan Controller initialized."));
}

/**
 * @brief Destroy the {@link TesLight::FanController} instance and free resources.
 */
TesLight::FanController::~FanController()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete fan controller."));
	ledcDetachPin(this->fanPin);
}

/**
 * @brief Set the temperature to calculate the new fan speed.
 * @param temp current temperature
 */
void TesLight::FanController::setTemperature(const uint8_t temp)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Handle fan controller."));

	// Convert the temperature into a value between 0 and 1 within the min/max limits
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Normalize temperature value."));
	float temperature = temp;
	if (temperature < this->tempMin)
	{
		temperature = this->tempMin;
	}
	else if (temperature > this->tempMax)
	{
		temperature = this->tempMax;
	}
	temperature = (temperature - this->tempMin) / (this->tempMax - this->tempMin);

	// Based on the temperature value, calculate the pwm value
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculate PWM value for fan."));
	uint16_t pwm = temperature == 0.0f ? 0.0f : (this->pwmMax - this->pwmMin) * temperature + this->pwmMin;
	if (pwm > this->pwmMax)
	{
		pwm = this->pwmMax;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Write PWM output."));
	ledcWrite(FAN_PWM_CHANNEL, pwm);
}
