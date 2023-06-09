/**
 * @file LightSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::LightSensor}.
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
#include "sensor/LightSensor.h"

bool NL::LightSensor::initialized = false;
float NL::LightSensor::lastBrightnessValue;
NL::MotionSensor::MotionSensorData NL::LightSensor::motionData;
unsigned long NL::LightSensor::motionSensorTriggerTime;

/**
 * @brief Start the light sensor.
 * @return OK when the light sensor was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration is not available
 */
NL::LightSensor::Error NL::LightSensor::begin()
{
	NL::LightSensor::initialized = false;
	if (!NL::Configuration::isInitialized())
	{
		return NL::LightSensor::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	NL::LightSensor::lastBrightnessValue = 0.0f;
	NL::LightSensor::motionData.accXRaw = 0;
	NL::LightSensor::motionData.accYRaw = 0;
	NL::LightSensor::motionData.accZRaw = 0;
	NL::LightSensor::motionData.gyroXRaw = 0;
	NL::LightSensor::motionData.gyroYRaw = 0;
	NL::LightSensor::motionData.gyroZRaw = 0;
	NL::LightSensor::motionData.accXG = 0.0f;
	NL::LightSensor::motionData.accYG = 0.0f;
	NL::LightSensor::motionData.accZG = 0.0f;
	NL::LightSensor::motionData.gyroXDeg = 0.0f;
	NL::LightSensor::motionData.gyroYDeg = 0.0f;
	NL::LightSensor::motionData.gyroZDeg = 0.0f;
	NL::LightSensor::motionData.pitch = 0.0f;
	NL::LightSensor::motionData.roll = 0.0f;
	NL::LightSensor::motionData.yaw = 0.0f;
	NL::LightSensor::motionData.rollCompensatedAccXG = 0.0f;
	NL::LightSensor::motionData.pitchCompensatedAccYG = 0.0f;
	NL::LightSensor::motionData.temperatureRaw = 0;
	NL::LightSensor::motionData.temperatureDeg = 0;
	NL::LightSensor::motionSensorTriggerTime = millis();

	NL::LightSensor::initialized = true;
	return NL::LightSensor::Error::OK;
}

/**
 * @brief Stop the light sensor.
 */
void NL::LightSensor::end()
{
	NL::LightSensor::initialized = false;
}

/**
 * @brief Check if the light sensor is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::LightSensor::isInitialized()
{
	return NL::LightSensor::initialized;
}

/**
 * @brief Return the LED brightness based on the sensors mode and the environment.
 * @param brightness 0.0 for minimum brightness up to 1.0 for maximum brightness
 * @return OK when the brightness was calculated
 * @return ERROR_ADC_UNAVAILABLE when the ADC is not available
 * @return ERROR_BH1750_UNAVAILABLE when the BH1750 is not available
 * @return ERROR_MPU6050_UNAVAILABLE when the MPU6050 is not available
 * @return ERROR_UNKNOWN_MODE when the light sensor mode is invalid
 */
NL::LightSensor::Error NL::LightSensor::getBrightness(float &brightness)
{
	brightness = NL::LightSensor::lastBrightnessValue;
	const NL::LightSensor::Error error = NL::LightSensor::getBrightnessInt(brightness);
	NL::LightSensor::lastBrightnessValue = brightness;
	return error;
}

/**
 * @brief Return the brightness of the lights based on the sensors mode.
 * @param brightness 0.0 for minimum brightness up to 1.0 for maximum brightness
 * @return OK when the brightness was calculated
 * @return ERROR_ADC_UNAVAILABLE when the ADC is not available
 * @return ERROR_BH1750_UNAVAILABLE when the BH1750 is not available
 * @return ERROR_MPU6050_UNAVAILABLE when the MPU6050 is not available
 * @return ERROR_UNKNOWN_MODE when the light sensor mode is invalid
 */
NL::LightSensor::Error NL::LightSensor::getBrightnessInt(float &brightness)
{
	const NL::Configuration::SystemConfig systemConfig = NL::Configuration::getSystemConfig();
	const float antiFlickerThreshold = 0.002f;
	const NL::LightSensor::LightSensorMode lightSensorMode = (NL::LightSensor::LightSensorMode)systemConfig.lightSensorMode;
	const float threshold = systemConfig.lightSensorThreshold / 255.0f;
	const float minAmbientBrightness = systemConfig.lightSensorMinAmbientBrightness / 255.0f;
	const float maxAmbientBrightness = systemConfig.lightSensorMaxAmbientBrightness / 255.0f;
	const float minLedBrightness = systemConfig.lightSensorMinLedBrightness / 255.0f;
	const float maxLedBrightness = systemConfig.lightSensorMaxLedBrightness / 255.0f;
	const uint8_t duration = systemConfig.lightSensorDuration;

	// Always off
	if (lightSensorMode == NL::LightSensor::LightSensorMode::ALWAYS_OFF)
	{
		brightness = 0.0f;
		return NL::LightSensor::Error::OK;
	}

	// Always on
	else if (lightSensorMode == NL::LightSensor::LightSensorMode::ALWAYS_ON)
	{
		brightness = 1.0f;
		return NL::LightSensor::Error::OK;
	}

	// Auto on/off using ADC
	else if (lightSensorMode == NL::LightSensor::LightSensorMode::AUTO_ON_OFF_ADC)
	{
		if (!NL::AnalogInput::isInitialized())
		{
			brightness = 1.0f;
			return NL::LightSensor::Error::ERROR_ADC_UNAVAILABLE;
		}

		float value = 0.0f;
		for (uint8_t i = 0; i < 10; i++)
		{
			value += NL::AnalogInput::getAnalogVoltage() / 3.3f;
		}
		value /= 10.0f;

		if (value > threshold + antiFlickerThreshold)
		{
			brightness = 1.0f;
		}
		else if (value < threshold - antiFlickerThreshold)
		{
			brightness = 0.0f;
		}

		return NL::LightSensor::Error::OK;
	}

	// Auto brightness using ADC
	else if (lightSensorMode == NL::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_ADC)
	{
		if (!NL::AnalogInput::isInitialized())
		{
			brightness = 1.0f;
			return NL::LightSensor::Error::ERROR_ADC_UNAVAILABLE;
		}

		float value = 0.0f;
		for (uint8_t i = 0; i < 10; i++)
		{
			value += NL::AnalogInput::getAnalogVoltage(false);
		}
		value /= 33.0f; // 3.3V * 10

		if (value < threshold - antiFlickerThreshold)
		{
			brightness = 0.0f;
			return NL::LightSensor::Error::OK;
		}
		else if (value > threshold + antiFlickerThreshold)
		{
			value = (value - minAmbientBrightness) / (maxAmbientBrightness - minAmbientBrightness);
			if (value < 0.0f)
			{
				value = 0.0f;
			}
			else if (value > 1.0f)
			{
				value = 1.0f;
			}

			brightness = minLedBrightness + value * (maxLedBrightness - minLedBrightness);
			if (brightness < 0.0f)
			{
				brightness = 0.0f;
			}
			else if (brightness > 1.0f)
			{
				brightness = 1.0f;
			}
		}

		return NL::LightSensor::Error::OK;
	}

	// Auto on/off using BH1750
	else if (lightSensorMode == NL::LightSensor::LightSensorMode::AUTO_ON_OFF_BH1750)
	{
		if (!NL::BH1750::isInitialized())
		{
			brightness = 1.0f;
			return NL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;
		}

		float lux = 0.0f;
		if (NL::BH1750::getLux(lux) != NL::BH1750::Error::OK)
		{
			brightness = 0.0f;
			return NL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;
		}

		lux /= 54612.5f / 5.0f;
		if (lux > 1.0f)
		{
			lux = 1.0f;
		}

		if (lux > threshold + antiFlickerThreshold)
		{
			brightness = 0.0f;
		}
		else if (lux < threshold - antiFlickerThreshold)
		{
			brightness = 1.0f;
		}

		return NL::LightSensor::Error::OK;
	}

	// Auto brightness using BH1750
	else if (lightSensorMode == NL::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_BH1750)
	{
		if (!NL::BH1750::isInitialized())
		{
			brightness = 1.0f;
			return NL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;
		}

		float lux = 0.0f;
		if (NL::BH1750::getLux(lux) != NL::BH1750::Error::OK)
		{
			brightness = 0.0f;
			return NL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;
		}

		lux /= 54612.5f / 5.0f;
		if (lux > 1.0f)
		{
			lux = 1.0f;
		}

		if (lux > threshold + antiFlickerThreshold)
		{
			brightness = 0.0f;
			return NL::LightSensor::Error::OK;
		}
		else if (lux < threshold - antiFlickerThreshold)
		{
			lux = (lux - minAmbientBrightness) / (maxAmbientBrightness - minAmbientBrightness);
			if (lux < 0.0f)
			{
				lux = 0.0f;
			}
			else if (lux > 1.0f)
			{
				lux = 1.0f;
			}

			brightness = minLedBrightness + lux * (maxLedBrightness - minLedBrightness);
			if (brightness < 0.0f)
			{
				brightness = 0.0f;
			}
			else if (brightness > 1.0f)
			{
				brightness = 1.0f;
			}
		}

		return NL::LightSensor::Error::OK;
	}

	// Auto on/off using motion sensor MPU6050
	else if (lightSensorMode == NL::LightSensor::LightSensorMode::AUTO_ON_OFF_MOTION)
	{
		if (!NL::MotionSensor::isInitialized())
		{
			brightness = 1.0f;
			return NL::LightSensor::Error::ERROR_MPU6050_UNAVAILABLE;
		}

		const NL::MotionSensor::MotionSensorData motionData = NL::MotionSensor::getMotion();
		float trigger = sqrt(pow(motionData.accXG * 150.0f - NL::LightSensor::motionData.accXG * 150.0f, 2) + pow(motionData.accYG * 150.0f - NL::LightSensor::motionData.accYG * 150.0f, 2) + pow(motionData.accZG * 150.0f - NL::LightSensor::motionData.accZG * 150.0f, 2));
		trigger += sqrt(pow(motionData.gyroXDeg - NL::LightSensor::motionData.gyroXDeg, 2) + pow(motionData.gyroYDeg - NL::LightSensor::motionData.gyroYDeg, 2) + pow(motionData.gyroZDeg - NL::LightSensor::motionData.gyroZDeg, 2));
		NL::LightSensor::motionData = motionData;

		if (trigger > threshold * 20.0f)
		{
			brightness = 1.0f;
			NL::LightSensor::motionSensorTriggerTime = millis();
		}
		else if (millis() - NL::LightSensor::motionSensorTriggerTime > duration * 5000L)
		{
			brightness = 0.0f;
		}

		return NL::LightSensor::Error::OK;
	}

	brightness = 1.0f;
	return NL::LightSensor::Error::ERROR_UNKNOWN_MODE;
}
