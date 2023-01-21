/**
 * @file Fan.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::Fan}.
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
#include "hardware/Fan.h"

bool TL::Fan::initialized = false;
uint8_t TL::Fan::fanPin;
uint8_t TL::Fan::pwmChannel;
uint32_t TL::Fan::frequency;
uint8_t TL::Fan::resolution;
uint8_t TL::Fan::pwmValue;

/**
 * @brief Initialize the {@link TL::Fan}. This required the {@link TL::Configuration} to be initialized beforehand.
 * @param fanPin output pin for the pwm signal to controll the fan
 * @param pwmChannel channel for the PWM output
 * @param frequency frequency of the pwm output
 * @param resolution resolution of the pwm output
 * @return OK when the fan controller was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration is not available
 * @return ERROR_SETUP_PIN when the output pin could not be configured
 */
TL::Fan::Error TL::Fan::begin(const uint8_t fanPin, const uint8_t pwmChannel, const uint32_t frequency, const uint8_t resolution)
{
	TL::Fan::initialized = false;
	TL::Fan::fanPin = fanPin;
	TL::Fan::pwmChannel = pwmChannel;
	TL::Fan::frequency = frequency;
	TL::Fan::resolution = resolution;
	TL::Fan::pwmValue = 0;

	if (!TL::Configuration::isInitialized())
	{
		return TL::Fan::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	if (ledcSetup(TL::Fan::pwmChannel, TL::Fan::frequency, TL::Fan::resolution) == 0)
	{
		return TL::Fan::Error::ERROR_SETUP_PIN;
	}
	ledcAttachPin(TL::Fan::fanPin, TL::Fan::pwmChannel);

	TL::Fan::initialized = true;
	return TL::Fan::Error::OK;
}

/**
 * @brief Stop the fan controller.
 */
void TL::Fan::end()
{
	TL::Fan::initialized = false;
	ledcDetachPin(TL::Fan::fanPin);
}

/**
 * @brief Return if the controller was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::Fan::isInitialized()
{
	return TL::Fan::initialized;
}

/**
 * @brief Run the fan speed control.
 * @param fanMode mode of the fan
 * @return OK when the fan speed was updated
 * @return ERROR_TEMP_UNAVAILABLE when the temperature could not be read
 * @return ERROR_UNKNOWN_MODE when the fan mode is unknown
 */
TL::Fan::Error TL::Fan::run(const TL::Fan::FanMode fanMode)
{
	TL::Fan::Error error = TL::Fan::Error::OK;
	const TL::Configuration::SystemConfig systemConfig = TL::Configuration::getSystemConfig();
	if (fanMode == TL::Fan::FanMode::AUTOMATIC)
	{
		float temperature;
		const TL::TemperatureSensor::Error tempError = TL::TemperatureSensor::getMaxTemperature(temperature);
		if (tempError == TL::TemperatureSensor::Error::OK)
		{
			if (temperature < systemConfig.fanMinTemperature)
			{
				temperature = systemConfig.fanMinTemperature;
			}
			else if (temperature > systemConfig.fanMaxTemperature)
			{
				temperature = systemConfig.fanMaxTemperature;
			}
			temperature = (temperature - systemConfig.fanMinTemperature) / (systemConfig.fanMaxTemperature - systemConfig.fanMinTemperature);

			TL::Fan::pwmValue = temperature == 0.0f ? 0 : (systemConfig.fanMaxPwmValue - systemConfig.fanMinPwmValue) * temperature + systemConfig.fanMinPwmValue;
			if (TL::Fan::pwmValue > systemConfig.fanMaxPwmValue)
			{
				TL::Fan::pwmValue = systemConfig.fanMaxPwmValue;
			}
		}
		else
		{
			error = TL::Fan::Error::ERROR_TEMP_UNAVAILABLE;
			TL::Fan::pwmValue = 255;
		}
	}
	else if (fanMode == TL::Fan::FanMode::MANUAL_OFF)
	{
		TL::Fan::pwmValue = 0;
	}
	else if (fanMode == TL::Fan::FanMode::MANUAL_25)
	{
		TL::Fan::pwmValue = 63;
	}
	else if (fanMode == TL::Fan::FanMode::MANUAL_50)
	{
		TL::Fan::pwmValue = 127;
	}
	else if (fanMode == TL::Fan::FanMode::MANUAL_75)
	{
		TL::Fan::pwmValue = 191;
	}
	else if (fanMode == TL::Fan::FanMode::MANUAL_100)
	{
		TL::Fan::pwmValue = 255;
	}
	else
	{
		error = TL::Fan::Error::ERROR_UNKNOWN_MODE;
	}

	ledcWrite(TL::Fan::pwmChannel, TL::Fan::pwmValue);
	return error;
}

/**
 * @brief Get the currently set pwm value.
 * @return currently set pwm value
 */
uint8_t TL::Fan::getPwmValue()
{
	return TL::Fan::pwmValue;
}
