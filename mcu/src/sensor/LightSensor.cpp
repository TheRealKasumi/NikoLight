/**
 * @file LightSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::LightSensor}.
 * @version 0.0.1
 * @date 2022-08-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/LightSensor.h"

/**
 * @brief Construct a new instance of {@link TesLight::LightSensor}.
 * @param inputPin input pin number to read the analog value
 * @param bufferSize buffer size for smoothing out values
 * @param threshold threshold to trigger the sensor
 * @param minValue minimum raw value from the sensor, representing 0.0 brightness
 * @param maxValue maximum raw value from the sensor, representing 1.0 brightness
 * @param lightSensorMode mode in which the light sensor operates
 */
TesLight::LightSensor::LightSensor(const uint8_t inputPin, const uint16_t bufferSize, const TesLight::LightSensor::LightSensorMode lightSensorMode, const uint16_t threshold, const uint16_t minValue, const uint16_t maxValue)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LightSensor.cpp:LightSensor"), F("Initializing Light Sensor."));
	this->inputPin = inputPin;
	this->bufferSize = bufferSize;

	this->lightSensorMode = lightSensorMode;
	this->threshold = threshold;
	this->minValue = minValue;
	this->maxValue = maxValue;

	this->buffer = new uint16_t[this->bufferSize];
	for (uint16_t i = 0; i < this->bufferSize; i++)
	{
		this->buffer[i] = 0;
	}
	this->bufferIndex = 0;

	pinMode(inputPin, INPUT);
}

/**
 * @brief Destroy the instance.
 */
TesLight::LightSensor::~LightSensor()
{
	delete[] this->buffer;
	this->buffer = nullptr;
}

/**
 * @brief Get the light sensor mode.
 * @return light sensor mode
 */
TesLight::LightSensor::LightSensorMode TesLight::LightSensor::getLightSensorMode()
{
	return this->lightSensorMode;
}

/**
 * @brief Set the light sensor mode.
 * @param lightSensorMode mode in which the light sensor should operate
 */
void TesLight::LightSensor::setLightSensorMode(const TesLight::LightSensor::LightSensorMode lightSensorMode)
{
	this->lightSensorMode = lightSensorMode;
}

/**
 * @brief Get the threshold for binary mode.
 * @return threshold value
 */
uint16_t TesLight::LightSensor::getThreshold()
{
	return this->threshold;
}

/**
 * @brief Set the threshold for binary mode.
 * @param threshold value
 */
void TesLight::LightSensor::setThreshold(const uint16_t threshold)
{
	this->threshold = threshold;
}

/**
 * @brief Get the min value for auto brighness mode.
 * @return min value representing 0.0 brighness
 */
uint16_t TesLight::LightSensor::getMinValue()
{
	return this->minValue;
}

/**
 * @brief Set the min value for auto brighness mode.
 * @param minValue representing 0.0 brighness
 */
void TesLight::LightSensor::setMinValue(const uint16_t minValue)
{
	this->minValue = minValue;
}

/**
 * @brief Get the max value for auto brighness mode.
 * @return max value representing 1.0 brighness
 */
uint16_t TesLight::LightSensor::getMaxValue()
{
	return this->maxValue;
}

/**
 * @brief Set the max value for auto brighness mode.
 * @param maxValue representing 1.0 brighness
 */
void TesLight::LightSensor::setMaxValue(const uint16_t maxValue)
{
	this->maxValue = maxValue;
}

/**
 * @brief Return the brightness of the lights based on the sensors mode.
 * @return 0.0 for minimum brightness up to 1.0 for maximum brightness
 */
float TesLight::LightSensor::getBrightness()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LightSensor.cpp:getBrightness"), F("Reading light brightness."));
	const uint16_t rawValue = this->getAnalogeValue();

	// Always off
	if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_OFF)
	{
		return 0.0f;
	}

	// Always on
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_ON)
	{
		return 1.0f;
	}

	// Auto on/off
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_ON_OFF)
	{
		return rawValue > this->threshold;
	}

	// Auto brightness
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_BRIGHTNESS)
	{
		float brightness = ((float)rawValue - this->minValue) / (this->maxValue - this->minValue);
		if (brightness < 0.0f)
		{
			brightness = 0.0f;
		}
		else if (brightness > 1.0f)
		{
			brightness = 1.0f;
		}
		return brightness;
	}

	return 0.0f;
}

/**
 * @brief Read a smoothed analog value.
 * @return smoothed analog reading
 */
uint16_t TesLight::LightSensor::getAnalogeValue()
{
	this->buffer[this->bufferIndex++] = analogRead(this->inputPin);
	if (this->bufferIndex == this->bufferSize)
	{
		this->bufferIndex = 0;
	}

	float average = 0.0f;
	for (uint16_t i = 0; i < this->bufferSize; i++)
	{
		average += buffer[i];
	}
	return average / this->bufferSize;
}