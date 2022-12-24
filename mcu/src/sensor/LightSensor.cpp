/**
 * @file LightSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::LightSensor}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/LightSensor.h"

/**
 * @brief Create a new instance of {@link TL::LightSensor}.
 * @param configuration reference to the configuration
 */
TL::LightSensor::LightSensor(TL::Configuration *configuration)
{
	this->configuration = configuration;
	this->esp32adc.reset(new TL::ESP32ADC(LIGHT_SENSOR_ADC_PIN, INPUT, 3.3f));
	this->bh1750.reset(new TL::BH1750(IIC_ADDRESS_BH1750));
	if (!this->bh1750->begin())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("The bh1750 light sensor was not found on the I²C bus and will not be available."));
		this->bh1750.reset();
	}
	this->lastBrightnessValue = 0.0f;
	this->motionData.accXRaw = 0;
	this->motionData.accYRaw = 0;
	this->motionData.accZRaw = 0;
	this->motionData.gyroXRaw = 0;
	this->motionData.gyroYRaw = 0;
	this->motionData.gyroZRaw = 0;
	this->motionData.accXG = 0.0f;
	this->motionData.accYG = 0.0f;
	this->motionData.accZG = 0.0f;
	this->motionData.gyroXDeg = 0.0f;
	this->motionData.gyroYDeg = 0.0f;
	this->motionData.gyroZDeg = 0.0f;
	this->motionData.pitch = 0.0f;
	this->motionData.roll = 0.0f;
	this->motionData.yaw = 0.0f;
	this->motionData.rollCompensatedAccXG = 0.0f;
	this->motionData.pitchCompensatedAccYG = 0.0f;
	this->motionData.temperatureRaw = 0;
	this->motionData.temperatureDeg = 0;
	this->motionSensorTriggerTime = millis();
}

/**
 * @brief Delete the {@link TL::LightSensor} instance.
 */
TL::LightSensor::~LightSensor()
{
}

/**
 * @brief Return the brightness of the lights based on the sensors mode.
 * @param brightness 0.0 for minimum brightness up to 1.0 for maximum brightness
 * @param motionSensor reference to a {@link TL::MotionSensor} instance
 * @return true when successful
 * @return false when there was an error
 */
bool TL::LightSensor::getBrightness(float &brightness, TL::MotionSensor *motionSensor)
{
	brightness = this->lastBrightnessValue;
	const bool success = this->getBrightnessInt(brightness, motionSensor);
	this->lastBrightnessValue = brightness;
	return success;
}

/**
 * @brief Return the brightness of the lights based on the sensors mode.
 * @param brightness 0.0 for minimum brightness up to 1.0 for maximum brightness
 * @param motionSensor reference to a {@link TL::MotionSensor} instance
 * @return true when successful
 * @return false when there was an error
 */
bool TL::LightSensor::getBrightnessInt(float &brightness, TL::MotionSensor *motionSensor)
{
	const TL::Configuration::SystemConfig systemConfig = this->configuration->getSystemConfig();
	const float antiFlickerThreshold = 0.002f;
	const TL::LightSensor::LightSensorMode lightSensorMode = (TL::LightSensor::LightSensorMode)systemConfig.lightSensorMode;
	const float threshold = systemConfig.lightSensorThreshold / 255.0f;
	const float minAmbientBrightness = systemConfig.lightSensorMinAmbientBrightness / 255.0f;
	const float maxAmbientBrightness = systemConfig.lightSensorMaxAmbientBrightness / 255.0f;
	const float minLedBrightness = systemConfig.lightSensorMinLedBrightness / 255.0f;
	const float maxLedBrightness = systemConfig.lightSensorMaxLedBrightness / 255.0f;
	const uint8_t duration = systemConfig.lightSensorDuration;

	// Always off
	if (lightSensorMode == TL::LightSensor::LightSensorMode::ALWAYS_OFF)
	{
		brightness = 0.0f;
		return true;
	}

	// Always on
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::ALWAYS_ON)
	{
		brightness = 1.0f;
		return true;
	}

	// Auto on/off using ADC
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_ON_OFF_ADC)
	{
		float value = 0.0f;
		for (uint8_t i = 0; i < 10; i++)
		{
			value += this->esp32adc->getAnalogVoltage() / 3.3f;
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

		return true;
	}

	// Auto brightness using ADC
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_ADC)
	{
		float value = 0.0f;
		for (uint8_t i = 0; i < 10; i++)
		{
			value += this->esp32adc->getAnalogVoltage(false);
		}
		value /= 33.0f; // 3.3V * 10

		if (value < threshold - antiFlickerThreshold)
		{
			brightness = 0.0f;
			return true;
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

		return true;
	}

	// Auto on/off using BH1750
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_ON_OFF_BH1750 && this->bh1750)
	{
		float lux = 0.0f;
		if (!this->bh1750->getLux(lux))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Light sensor mode is AUTO_ON_OFF_BH1750 but the BH1750 sensor was not found on the I²C bus."));
			brightness = 1.0f;
			return false;
		}
		lux /= 54612.5f;

		if (lux > threshold + antiFlickerThreshold)
		{
			brightness = 0.0f;
		}
		else if (lux < threshold - antiFlickerThreshold)
		{
			brightness = 1.0f;
		}

		return true;
	}

	// Auto brightness using BH1750
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_BH1750 && this->bh1750)
	{
		float lux = 0.0f;
		if (!this->bh1750->getLux(lux))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Light sensor mode is AUTO_BRIGHTNESS_BH1750 but the BH1750 sensor was not found on the I²C bus."));
			brightness = 1.0f;
			return false;
		}
		lux /= 54612.5f;

		if (lux > threshold + antiFlickerThreshold)
		{
			brightness = 0.0f;
			return true;
		}
		else if (lux < threshold - antiFlickerThreshold)
		{
			lux = 1.0f - ((lux - minAmbientBrightness) / (maxAmbientBrightness - minAmbientBrightness));
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

		return true;
	}

	// Auto on/off using motion sensor mpu6050
	else if (lightSensorMode == TL::LightSensor::LightSensorMode::AUTO_ON_OFF_MPU6050 && motionSensor)
	{
		const TL::MotionSensor::MotionSensorData motionData = motionSensor->getMotion();
		float trigger = sqrt(pow(motionData.accXG * 150.0f - this->motionData.accXG * 150.0f, 2) + pow(motionData.accYG * 150.0f - this->motionData.accYG * 150.0f, 2) + pow(motionData.accZG * 150.0f - this->motionData.accZG * 150.0f, 2));
		trigger += sqrt(pow(motionData.gyroXDeg - this->motionData.gyroXDeg, 2) + pow(motionData.gyroYDeg - this->motionData.gyroYDeg, 2) + pow(motionData.gyroZDeg - this->motionData.gyroZDeg, 2));
		this->motionData = motionData;

		if (trigger > threshold * 20.0f)
		{
			brightness = 1.0f;
			this->motionSensorTriggerTime = millis();
		}
		else if (millis() - this->motionSensorTriggerTime > duration * 5000L)
		{
			brightness = 0.0f;
		}

		return true;
	}

	TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Light sensor mode is unknown or invalid."));
	brightness = 0.0f;
	return false;
}
