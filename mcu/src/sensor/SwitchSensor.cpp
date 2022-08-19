/**
 * @file SwitchSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::SwitchSensor}.
 * @version 0.0.1
 * @date 2022-08-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/SwitchSensor.h"

/**
 * @brief Construct a new instance of {@link TesLight::SwitchSensor}.
 * @param inputPin input pin number to read the analog value
 * @param threshold threshold to trigger the sensor
 */
TesLight::SwitchSensor::SwitchSensor(const uint8_t inputPin, uint16_t threshold)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("SwitchSensor.cpp:SwitchSensor"), F("Initializing Switch Sensor."));
	this->inputPin = inputPin;
	this->threshold = threshold;
	pinMode(inputPin, INPUT);
}

/**
 * @brief Destroy the instance.
 */
TesLight::SwitchSensor::~SwitchSensor()
{
}

/**
 * @brief Check the state of the sensor based on the input value
 * @return true when active
 * @return false when inactive
 */
bool TesLight::SwitchSensor::isActive()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("SwitchSensor.cpp:isActive"), F("Checking switch Sensor state."));
	return analogRead(this->inputPin) > this->threshold;
}