/**
 * @file LightSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::LightSensor}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/LightSensor.h"

/**
 * @brief Create a new instance of {@link TesLight::LightSensor}.
 * @param configuration reference to the configuration
 */
TesLight::LightSensor::LightSensor(TesLight::Configuration *configuration)
{
	this->configuration = configuration;
	this->esp32adc = new TesLight::ESP32ADC(LIGHT_SENSOR_ADC_PIN, INPUT, 3.3f);
	this->bh1750 = new TesLight::BH1750(IIC_ADDRESS_BH1750);
	if (!this->bh1750->begin())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("The bh1750 light sensor was not found on the I²C bus and will not be available."));
		delete this->bh1750;
		this->bh1750 = nullptr;
	}
}

/**
 * @brief Delete the {@link TesLight::LightSensor} instance and free resoucres.
 */
TesLight::LightSensor::~LightSensor()
{
	delete this->esp32adc;
	this->esp32adc = nullptr;
	if (this->bh1750)
	{
		delete this->bh1750;
		this->bh1750 = nullptr;
	}
}

/**
 * @brief Return the brightness of the lights based on the sensors mode.
 * @param brightness 0.0 for minimum brightness up to 1.0 for maximum brightness
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LightSensor::getBrightness(float &brightness)
{
	const TesLight::Configuration::SystemConfig systemConfig = this->configuration->getSystemConfig();
	const float antiFlickerThreshold = 0.002f;
	const TesLight::LightSensor::LightSensorMode lightSensorMode = (TesLight::LightSensor::LightSensorMode)systemConfig.lightSensorMode;
	const float threshold = systemConfig.lightSensorThreshold / 255.0f;
	const float minAmbientBrightness = systemConfig.lightSensorMinAmbientBrightness / 255.0f;
	const float maxAmbientBrightness = systemConfig.lightSensorMaxAmbientBrightness / 255.0f;
	const float minLedBrightness = systemConfig.lightSensorMinLedBrightness / 255.0f;
	const float maxLedBrightness = systemConfig.lightSensorMaxLedBrightness / 255.0f;

	// Always off
	if (lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_OFF)
	{
		brightness = 0.0f;
		return true;
	}

	// Always on
	else if (lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_ON)
	{
		brightness = 1.0f;
		return true;
	}

	// Auto on/off using ADC
	else if (lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_ON_OFF_ADC)
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
	else if (lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_ADC)
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
	else if (lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_ON_OFF_BH1750 && this->bh1750)
	{
		float lux = 0.0f;
		if (!this->bh1750->getLux(lux))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Light sensor mode is AUTO_ON_OFF_BH1750 but the BH1750 sensor was not found on the I²C bus."));
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
	else if (lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_BH1750 && this->bh1750)
	{
		float lux = 0.0f;
		if (!this->bh1750->getLux(lux))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Light sensor mode is AUTO_BRIGHTNESS_BH1750 but the BH1750 sensor was not found on the I²C bus."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Light sensor mode is unknown or invalid."));
	brightness = 0.0f;
	return false;
}
