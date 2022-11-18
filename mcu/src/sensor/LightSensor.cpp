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
 * @param lightSensorMode mode in which the light sensor operates
 * @param threshold threshold to turn on the output
 * @param minValue minimum value from the sensor, representing 0.0 brightness
 * @param maxValue maximum value from the sensor, representing 1.0 brightness
 */
TesLight::LightSensor::LightSensor(const TesLight::LightSensor::LightSensorMode lightSensorMode, const float threshold, const float minValue, const float maxValue)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize light sensor."));
	this->esp32adc = new TesLight::ESP32ADC(LIGHT_SENSOR_ADC_PIN, INPUT, 3.3f);
	this->bh1750 = new TesLight::BH1750(IIC_ADDRESS_BH1750);
	this->lightSensorMode = lightSensorMode;
	this->threshold = threshold;
	this->minValue = minValue;
	this->maxValue = maxValue;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Starting bh1750 light sensor."));
	if (!this->bh1750->begin())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("The bh1750 light sensor was not found on the I²C bus and will not be available."));
		delete this->bh1750;
		this->bh1750 = nullptr;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("The bh1750 light sensor was started."));
	}
}

/**
 * @brief Delete the {@link TesLight::LightSensor} instance and free resoucres.
 */
TesLight::LightSensor::~LightSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete light sensor and free resources."));
	delete this->esp32adc;
	this->esp32adc = nullptr;

	if (this->bh1750)
	{
		delete this->bh1750;
		this->bh1750 = nullptr;
	}
}

/**
 * @brief Get the currently set light sensor mode.
 * @return light sensor mode
 */
TesLight::LightSensor::LightSensorMode TesLight::LightSensor::getLightSensorMode()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get light sensor mode."));
	return this->lightSensorMode;
}

/**
 * @brief Set the light sensor mode.
 * @param lightSensorMode mode in which the light sensor is operating
 */
void TesLight::LightSensor::setLightSensorMode(const TesLight::LightSensor::LightSensorMode lightSensorMode)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set light sensor mode to ") + lightSensorMode + F("."));
	this->lightSensorMode = lightSensorMode;
}

/**
 * @brief Get the threshold for binary mode.
 * @return threshold value
 */
float TesLight::LightSensor::getThreshold()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get light sensor threshold."));
	return this->threshold;
}

/**
 * @brief Set the threshold for binary mode.
 * @param threshold value
 */
void TesLight::LightSensor::setThreshold(const float threshold)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set light sensor threshold to ") + threshold + F("."));
	this->threshold = threshold;
}

/**
 * @brief Get the min value for auto brighness mode.
 * @return min value representing 0.0 brighness
 */
float TesLight::LightSensor::getMinValue()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get light sensor min value."));
	return this->minValue;
}

/**
 * @brief Set the min value for auto brighness mode.
 * @param minValue representing 0.0 brighness
 */
void TesLight::LightSensor::setMinValue(const float minValue)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set light sensor min value to ") + minValue + F("."));
	this->minValue = minValue;
}

/**
 * @brief Get the max value for auto brighness mode.
 * @return max value representing 1.0 brighness
 */
float TesLight::LightSensor::getMaxValue()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get light sensor max value."));
	return this->maxValue;
}

/**
 * @brief Set the max value for auto brighness mode.
 * @param maxValue representing 1.0 brighness
 */
void TesLight::LightSensor::setMaxValue(const float maxValue)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set light sensor max value to ") + maxValue + F("."));
	this->maxValue = maxValue;
}

/**
 * @brief Return the brightness of the lights based on the sensors mode.
 * @param brightness 0.0 for minimum brightness up to 1.0 for maximum brightness
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LightSensor::getBrightness(float &brightness)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading brightness from light sensor depending on the selected mode."));
	const float antiFlickerThreshold = 0.002f;

	// Always off
	if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_OFF)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light sensor mode is ALWAYS_OFF. The resulting brightness is 0.0."));
		brightness = 0.0f;
		return true;
	}

	// Always on
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_ON)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light sensor mode is ALWAYS_ON. The resulting brightness is 1.0."));
		brightness = 1.0f;
		return true;
	}

	// Auto on/off using ADC
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_ON_OFF_ADC)
	{
		const float value = this->esp32adc->getAnalogVoltage() / 3.3f;
		if (value > this->threshold + antiFlickerThreshold)
		{
			brightness = 1.0f;
		}
		else if (value < this->threshold - antiFlickerThreshold)
		{
			brightness = 0.0f;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Light sensor mode is AUTO_ON_OFF_ADC. The resulting brightness is ") + brightness + F("."));
		return true;
	}

	// Auto brightness using ADC
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_ADC)
	{
		const float value = this->esp32adc->getAnalogVoltage() / 3.3f;
		const float m = 1.0f / (this->maxValue - this->minValue);
		const float n = -(m * this->maxValue - 1);
		brightness = m * value + n + this->threshold;
		if (brightness < this->threshold)
		{
			brightness = this->threshold;
		}
		else if (brightness > 1.0f)
		{
			brightness = 1.0f;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Light sensor mode is AUTO_BRIGHTNESS_ADC. The resulting brightness is ") + brightness + F("."));
		return truncate;
	}

	// Auto on/off using BH1750
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_ON_OFF_BH1750 && this->bh1750)
	{
		float lux = 0.0f;
		if (!this->bh1750->getLux(lux))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Light sensor mode is AUTO_ON_OFF_BH1750 but the BH1750 sensor was not found on the I²C bus."));
			brightness = 1.0f;
			return false;
		}

		const float value = lux / 54612.0f;
		if (value > this->threshold + antiFlickerThreshold)
		{
			brightness = 0.0f;
		}
		else if (value < this->threshold - antiFlickerThreshold)
		{
			brightness = 1.0f;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Light sensor mode is AUTO_ON_OFF_BH1750. The resulting brightness is ") + brightness + F("."));
		return true;
	}

	// Auto brightness using BH1750
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_BRIGHTNESS_BH1750 && this->bh1750)
	{
		float lux = 0.0f;
		if (!this->bh1750->getLux(lux))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Light sensor mode is AUTO_BRIGHTNESS_BH1750 but the BH1750 sensor did not respond on the I²C bus."));
			brightness = 1.0f;
			return false;
		}

		const float value = lux / 54612.0f;
		const float m = 1.0f / (this->maxValue - this->minValue);
		const float n = -(m * this->maxValue - 1);
		brightness = m * value + n + this->threshold;
		if (brightness < this->threshold)
		{
			brightness = this->threshold;
		}
		else if (brightness > 1.0f)
		{
			brightness = 1.0f;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Light sensor mode is AUTO_BRIGHTNESS_BH1750. The resulting brightness is ") + brightness + F("."));
		return true;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Light sensor mode is unknown or invalid. The resulting brightness is 0.0."));
	brightness = 0.0f;
	return false;
}
