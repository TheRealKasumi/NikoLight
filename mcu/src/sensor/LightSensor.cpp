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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initialize Light Sensor at pin ") + String(this->inputPin) + F("."));
	this->inputPin = inputPin;
	this->bufferSize = bufferSize;

	this->lightSensorMode = lightSensorMode;
	this->threshold = threshold;
	this->minValue = minValue;
	this->maxValue = maxValue;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initialize light sensor buffer with size ") + String(this->bufferSize) + F("."));
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Deleting Light Sensor instance."));
	delete[] this->buffer;
	this->buffer = nullptr;
}

/**
 * @brief Get the light sensor mode.
 * @return light sensor mode
 */
TesLight::LightSensor::LightSensorMode TesLight::LightSensor::getLightSensorMode()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get Light Sensor mode."));
	return this->lightSensorMode;
}

/**
 * @brief Set the light sensor mode.
 * @param lightSensorMode mode in which the light sensor should operate
 */
void TesLight::LightSensor::setLightSensorMode(const TesLight::LightSensor::LightSensorMode lightSensorMode)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set Light Sensor mode to ") + String(lightSensorMode) + F("."));
	this->lightSensorMode = lightSensorMode;
}

/**
 * @brief Get the threshold for binary mode.
 * @return threshold value
 */
uint16_t TesLight::LightSensor::getThreshold()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get Light Sensor threshold."));
	return this->threshold;
}

/**
 * @brief Set the threshold for binary mode.
 * @param threshold value
 */
void TesLight::LightSensor::setThreshold(const uint16_t threshold)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Get Light Sensor threshold to ") + String(threshold) + F("."));
	this->threshold = threshold;
}

/**
 * @brief Get the min value for auto brighness mode.
 * @return min value representing 0.0 brighness
 */
uint16_t TesLight::LightSensor::getMinValue()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get Light Sensor min value."));
	return this->minValue;
}

/**
 * @brief Set the min value for auto brighness mode.
 * @param minValue representing 0.0 brighness
 */
void TesLight::LightSensor::setMinValue(const uint16_t minValue)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set Light Sensor min value to ") + String(minValue) + F("."));
	this->minValue = minValue;
}

/**
 * @brief Get the max value for auto brighness mode.
 * @return max value representing 1.0 brighness
 */
uint16_t TesLight::LightSensor::getMaxValue()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get Light Sensor max value."));
	return this->maxValue;
}

/**
 * @brief Set the max value for auto brighness mode.
 * @param maxValue representing 1.0 brighness
 */
void TesLight::LightSensor::setMaxValue(const uint16_t maxValue)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set Light Sensor max value to ") + String(maxValue) + F("."));
	this->maxValue = maxValue;
}

/**
 * @brief Return the brightness of the lights based on the sensors mode.
 * @return 0.0 for minimum brightness up to 1.0 for maximum brightness
 */
float TesLight::LightSensor::getBrightness()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading Light Sensor average value."));
	const uint16_t rawValue = this->getAnalogeValue();

	// Always off
	if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_OFF)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light Sensor mode is ALWAYS_OFF. The resulting value is 0.0."));
		return 0.0f;
	}

	// Always on
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::ALWAYS_ON)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light Sensor mode is ALWAYS_ON. The resulting value is 1.0."));
		return 1.0f;
	}

	// Auto on/off
	else if (this->lightSensorMode == TesLight::LightSensor::LightSensorMode::AUTO_ON_OFF)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Light Sensor mode is AUTO_ON_OFF. The resulting value is ") + String(rawValue > this->threshold ? 1.0f : 0.0f) + F("."));
		return rawValue > this->threshold ? 1.0f : 0.0f;
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

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Light Sensor mode is AUTO_BRIGHTNESS. The resulting value is ") + String(brightness) + F("."));
		return brightness;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light Sensor mode is unknown. The resulting value is 0.0."));
	return 0.0f;
}

/**
 * @brief Read a smoothed analog value.
 * @return smoothed analog reading
 */
uint16_t TesLight::LightSensor::getAnalogeValue()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading Light Sensor analog value and adding it to the buffer."));
	this->buffer[this->bufferIndex++] = analogRead(this->inputPin);
	if (this->bufferIndex == this->bufferSize)
	{
		this->bufferIndex = 0;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculating average value based on buffered values."));
	float average = 0.0f;
	for (uint16_t i = 0; i < this->bufferSize; i++)
	{
		average += buffer[i];
	}
	average /= this->bufferSize;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Average value is ") + String(average) + F("."));
	return average;
}