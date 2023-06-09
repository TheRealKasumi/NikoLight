/**
 * @file Fan.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::Fan}.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
