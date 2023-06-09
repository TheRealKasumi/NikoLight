/**
 * @file Fan.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::Fan}.
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

bool NL::Fan::initialized = false;
uint8_t NL::Fan::fanPin;
uint8_t NL::Fan::pwmChannel;
uint32_t NL::Fan::frequency;
uint8_t NL::Fan::resolution;
uint8_t NL::Fan::pwmValue;

/**
 * @brief Initialize the {@link NL::Fan}. This required the {@link NL::Configuration} to be initialized beforehand.
 * @param fanPin output pin for the pwm signal to controll the fan
 * @param pwmChannel channel for the PWM output
 * @param frequency frequency of the pwm output
 * @param resolution resolution of the pwm output
 * @return OK when the fan controller was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration is not available
 * @return ERROR_SETUP_PIN when the output pin could not be configured
 */
NL::Fan::Error NL::Fan::begin(const uint8_t fanPin, const uint8_t pwmChannel, const uint32_t frequency, const uint8_t resolution)
{
	NL::Fan::initialized = false;
	NL::Fan::fanPin = fanPin;
	NL::Fan::pwmChannel = pwmChannel;
	NL::Fan::frequency = frequency;
	NL::Fan::resolution = resolution;
	NL::Fan::pwmValue = 0;

	if (!NL::Configuration::isInitialized())
	{
		return NL::Fan::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	if (ledcSetup(NL::Fan::pwmChannel, NL::Fan::frequency, NL::Fan::resolution) == 0)
	{
		return NL::Fan::Error::ERROR_SETUP_PIN;
	}
	ledcAttachPin(NL::Fan::fanPin, NL::Fan::pwmChannel);

	NL::Fan::initialized = true;
	return NL::Fan::Error::OK;
}

/**
 * @brief Stop the fan controller.
 */
void NL::Fan::end()
{
	NL::Fan::initialized = false;
	ledcDetachPin(NL::Fan::fanPin);
}

/**
 * @brief Return if the controller was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::Fan::isInitialized()
{
	return NL::Fan::initialized;
}

/**
 * @brief Run the fan speed control.
 * @param fanMode mode of the fan
 * @return OK when the fan speed was updated
 * @return ERROR_TEMP_UNAVAILABLE when the temperature could not be read
 * @return ERROR_UNKNOWN_MODE when the fan mode is unknown
 */
NL::Fan::Error NL::Fan::run(const NL::Fan::FanMode fanMode)
{
	NL::Fan::Error error = NL::Fan::Error::OK;
	const NL::Configuration::SystemConfig systemConfig = NL::Configuration::getSystemConfig();
	if (fanMode == NL::Fan::FanMode::AUTOMATIC)
	{
		float temperature;
		const NL::TemperatureSensor::Error tempError = NL::TemperatureSensor::getMaxTemperature(temperature);
		if (tempError == NL::TemperatureSensor::Error::OK)
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

			NL::Fan::pwmValue = temperature == 0.0f ? 0 : (systemConfig.fanMaxPwmValue - systemConfig.fanMinPwmValue) * temperature + systemConfig.fanMinPwmValue;
			if (NL::Fan::pwmValue > systemConfig.fanMaxPwmValue)
			{
				NL::Fan::pwmValue = systemConfig.fanMaxPwmValue;
			}
		}
		else
		{
			error = NL::Fan::Error::ERROR_TEMP_UNAVAILABLE;
			NL::Fan::pwmValue = 255;
		}
	}
	else if (fanMode == NL::Fan::FanMode::MANUAL_OFF)
	{
		NL::Fan::pwmValue = 0;
	}
	else if (fanMode == NL::Fan::FanMode::MANUAL_25)
	{
		NL::Fan::pwmValue = 63;
	}
	else if (fanMode == NL::Fan::FanMode::MANUAL_50)
	{
		NL::Fan::pwmValue = 127;
	}
	else if (fanMode == NL::Fan::FanMode::MANUAL_75)
	{
		NL::Fan::pwmValue = 191;
	}
	else if (fanMode == NL::Fan::FanMode::MANUAL_100)
	{
		NL::Fan::pwmValue = 255;
	}
	else
	{
		error = NL::Fan::Error::ERROR_UNKNOWN_MODE;
	}

	ledcWrite(NL::Fan::pwmChannel, NL::Fan::pwmValue);
	return error;
}

/**
 * @brief Get the currently set pwm value.
 * @return currently set pwm value
 */
uint8_t NL::Fan::getPwmValue()
{
	return NL::Fan::pwmValue;
}
