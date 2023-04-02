/**
 * @file LightSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::LightSensor}.
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
#include "sensor/LightSensor.h"

bool TL::LightSensor::initialized = false;
float TL::LightSensor::lastBrightnessValue;
TL::MotionSensor::MotionSensorData TL::LightSensor::motionData;
unsigned long TL::LightSensor::motionSensorTriggerTime;

/**
 * @brief Start the light sensor.
 * @return OK when the light sensor was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration is not available
 */
TL::LightSensor::Error TL::LightSensor::begin()
{
	TL::LightSensor::initialized = false;
	if (!TL::Configuration::isInitialized())
	{
		return TL::LightSensor::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	TL::LightSensor::lastBrightnessValue = 0.0f;
	TL::LightSensor::motionData.accXRaw = 0;
	TL::LightSensor::motionData.accYRaw = 0;
	TL::LightSensor::motionData.accZRaw = 0;
	TL::LightSensor::motionData.gyroXRaw = 0;
	TL::LightSensor::motionData.gyroYRaw = 0;
	TL::LightSensor::motionData.gyroZRaw = 0;
	TL::LightSensor::motionData.accXG = 0.0f;
	TL::LightSensor::motionData.accYG = 0.0f;
	TL::LightSensor::motionData.accZG = 0.0f;
	TL::LightSensor::motionData.gyroXDeg = 0.0f;
	TL::LightSensor::motionData.gyroYDeg = 0.0f;
	TL::LightSensor::motionData.gyroZDeg = 0.0f;
	TL::LightSensor::motionData.pitch = 0.0f;
	TL::LightSensor::motionData.roll = 0.0f;
	TL::LightSensor::motionData.yaw = 0.0f;
	TL::LightSensor::motionData.rollCompensatedAccXG = 0.0f;
	TL::LightSensor::motionData.pitchCompensatedAccYG = 0.0f;
	TL::LightSensor::motionData.temperatureRaw = 0;
	TL::LightSensor::motionData.temperatureDeg = 0;
	TL::LightSensor::motionSensorTriggerTime = millis();

	TL::LightSensor::initialized = true;
	return TL::LightSensor::Error::OK;
}

/**
 * @brief Stop the light sensor.
 */
void TL::LightSensor::end()
{
	TL::LightSensor::initialized = false;
}

/**
 * @brief Check if the light sensor is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::LightSensor::isInitialized()
{
	return TL::LightSensor::initialized;
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
TL::LightSensor::Error TL::LightSensor::getBrightness(float &brightness)
{
	brightness = TL::LightSensor::lastBrightnessValue;
	const TL::LightSensor::Error error = TL::LightSensor::getBrightnessInt(brightness);
	TL::LightSensor::lastBrightnessValue = brightness;
	return error;
}

/**
 * @brief Read lux measurement from sensor and adjust brightness if sensor data is unavailable
 * @param lux 0.0 for minimum measured brightness up to 1.0 for maximum measured brightness
 * @param brightness 0.0 for minimum brightness up to 1.0 for maximum brightness
 * @return OK when brightness was successfully measured
 * @return ERROR_BH1750_UNAVAILABLE when the BH1750 is not available
*/
TL::LightSensor::Error TL::LightSensor::getLuxMeasurement(float &lux, float &brightness)
{
	if (!TL::BH1750::isInitialized())
	{
		// Turn brightness up while sensor is initializing or unavailable
		brightness = 1.0f;
		return TL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;
	}

	lux = 0.0f;
	if (TL::BH1750::getLux(lux) != TL::BH1750::Error::OK)
	{
		brightness = 0.0f;
		return TL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;
	}

	lux /= 54612.5f / 5.0f;
	if (lux > 1.0f) lux = 1.0f;

	return TL::LightSensor::Error::OK;
}

/**
 * @brief Set the brightness of the lights based on value and system config
*/
void TL::LightSensor::adjustBrightnessAccordingToConfig(float &value, float &brightness)
{
	const TL::Configuration::SystemConfig systemConfig = TL::Configuration::getSystemConfig();
	const float minAmbientBrightness = systemConfig.lightSensorMinAmbientBrightness / 255.0f;
	const float maxAmbientBrightness = systemConfig.lightSensorMaxAmbientBrightness / 255.0f;
	const float minLedBrightness = systemConfig.lightSensorMinLedBrightness / 255.0f;
	const float maxLedBrightness = systemConfig.lightSensorMaxLedBrightness / 255.0f;

	value = (value - minAmbientBrightness) / (maxAmbientBrightness - minAmbientBrightness);
	if (value < 0.0f) value = 0.0f;
	else if (value > 1.0f) value = 1.0f;

	brightness = minLedBrightness + value * (maxLedBrightness - minLedBrightness);
	if (brightness < 0.0f) brightness = 0.0f;
	else if (brightness > 1.0f) brightness = 1.0f;
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
TL::LightSensor::Error TL::LightSensor::getBrightnessInt(float &brightness)
{
	const TL::Configuration::SystemConfig systemConfig = TL::Configuration::getSystemConfig();
	const float antiFlickerThreshold = 0.002f;
	const TL::LightSensor::LightSensorMode lightSensorMode = (TL::LightSensor::LightSensorMode)systemConfig.lightSensorMode;
	const float threshold = systemConfig.lightSensorThreshold / 255.0f;
	const uint8_t duration = systemConfig.lightSensorDuration;

	// Always off
	if (lightSensorMode == TL::LightSensor::LightSensorMode::ALWAYS_OFF)
	{
		brightness = 0.0f;
		return TL::LightSensor::Error::OK;
	}

	// Always on
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::ALWAYS_ON)
	{
		brightness = 1.0f;
		return TL::LightSensor::Error::OK;
	}

	// Auto on/off using ADC
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_ON_OFF_ADC)
	{
		if (!TL::AnalogInput::isInitialized())
		{
			brightness = 1.0f;
			return TL::LightSensor::Error::ERROR_ADC_UNAVAILABLE;
		}

		float value = 0.0f;
		for (uint8_t i = 0; i < 10; i++)
		{
			value += TL::AnalogInput::getAnalogVoltage() / 3.3f;
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

		return TL::LightSensor::Error::OK;
	}

	// Auto brightness using ADC
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_ADC)
	{
		if (!TL::AnalogInput::isInitialized())
		{
			brightness = 1.0f;
			return TL::LightSensor::Error::ERROR_ADC_UNAVAILABLE;
		}

		float value = 0.0f;
		for (uint8_t i = 0; i < 10; i++)
		{
			value += TL::AnalogInput::getAnalogVoltage(false);
		}
		value /= 33.0f; // 3.3V * 10

		if (value < threshold - antiFlickerThreshold)
		{
			brightness = 0.0f;
			return TL::LightSensor::Error::OK;
		}
		else if (value > threshold + antiFlickerThreshold)
			TL::LightSensor::adjustBrightnessAccordingToConfig(value, brightness);

		return TL::LightSensor::Error::OK;
	}

	// Auto on/off using BH1750
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_ON_OFF_BH1750)
	{
		float lux = 0.0f;
		if(TL::LightSensor::getLuxMeasurement(lux, brightness) != TL::LightSensor::Error::OK)
			return TL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;

		if (lux > threshold + antiFlickerThreshold)
		{
			brightness = 0.0f;
		}
		else if (lux < threshold - antiFlickerThreshold)
		{
			brightness = 1.0f;
		}

		return TL::LightSensor::Error::OK;
	}

	// Auto brightness using BH1750
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_BH1750)
	{
		float lux = 0.0f;
		if(TL::LightSensor::getLuxMeasurement(lux, brightness) != TL::LightSensor::Error::OK)
			return TL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;

		if (lux > threshold + antiFlickerThreshold)
		{
			brightness = 0.0f;
			return TL::LightSensor::Error::OK;
		}
		else if (lux < threshold - antiFlickerThreshold)
		{
			TL::LightSensor::adjustBrightnessAccordingToConfig(lux, brightness);
		}

		return TL::LightSensor::Error::OK;
	}

	// Auto on/off using motion sensor MPU6050
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_ON_OFF_MOTION)
	{
		if (!TL::MotionSensor::isInitialized())
		{
			brightness = 1.0f;
			return TL::LightSensor::Error::ERROR_MPU6050_UNAVAILABLE;
		}

		const TL::MotionSensor::MotionSensorData motionData = TL::MotionSensor::getMotion();
		float trigger = sqrt(pow(motionData.accXG * 150.0f - TL::LightSensor::motionData.accXG * 150.0f, 2) + pow(motionData.accYG * 150.0f - TL::LightSensor::motionData.accYG * 150.0f, 2) + pow(motionData.accZG * 150.0f - TL::LightSensor::motionData.accZG * 150.0f, 2));
		trigger += sqrt(pow(motionData.gyroXDeg - TL::LightSensor::motionData.gyroXDeg, 2) + pow(motionData.gyroYDeg - TL::LightSensor::motionData.gyroYDeg, 2) + pow(motionData.gyroZDeg - TL::LightSensor::motionData.gyroZDeg, 2));
		TL::LightSensor::motionData = motionData;

		if (trigger > threshold * 20.0f)
		{
			brightness = 1.0f;
			TL::LightSensor::motionSensorTriggerTime = millis();
		}
		else if (millis() - TL::LightSensor::motionSensorTriggerTime > duration * 5000L)
		{
			brightness = 0.0f;
		}

		return TL::LightSensor::Error::OK;
	}

	// Auto on/off using ADC + auto brightness using BH1070
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_ON_OFF_ADC_AUTO_BRIGHTNESS_BH1750)
	{
		if (!TL::AnalogInput::isInitialized())
		{
			brightness = 1.0f;
			return TL::LightSensor::Error::ERROR_ADC_UNAVAILABLE;
		}

		float value = 0.0f;
		for (uint8_t i = 0; i < 10; i++)
		{
			value += TL::AnalogInput::getAnalogVoltage() / 3.3f;
		}
		value /= 10.0f;

		if (value > threshold + antiFlickerThreshold)
		{
			// Lights should be on (ADC)
			// Determine brightness using BH1070
			float lux = 0.0f;
			if(TL::LightSensor::getLuxMeasurement(lux, brightness) != TL::LightSensor::Error::OK)
			{
				brightness = 0.0f;
				return TL::LightSensor::Error::ERROR_BH1750_UNAVAILABLE;
			}

			TL::LightSensor::adjustBrightnessAccordingToConfig(lux, brightness);
		}
		else if (value < threshold - antiFlickerThreshold)
		{
			brightness = 0.0f;
		}

		return TL::LightSensor::Error::OK;
	}

	brightness = 1.0f;
	return TL::LightSensor::Error::ERROR_UNKNOWN_MODE;
}
