/**
 * @file ESP32ADC.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::ESP32ADC}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "hardware/ESP32ADC.h"

/**
 * @brief Create a new instance of {@link TesLight::ESP32ADC}.
 * @param inputPin physical input pin
 */
TesLight::ESP32ADC::ESP32ADC(const uint8_t inputPin)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize ESP32ADC."));
	this->inputPin = inputPin;
	this->inputMode = INPUT;
	this->maxVoltage = 3.3f;
	this->setupPin();
}

/**
 * @brief Create a new instance of {@link TesLight::ESP32ADC}.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
TesLight::ESP32ADC::ESP32ADC(const uint8_t inputPin, const uint8_t inputMode)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize ESP32ADC."));
	this->inputPin = inputPin;
	this->inputMode = inputMode;
	this->maxVoltage = 3.3f;
	this->setupPin();
}

/**
 * @brief Create a new instance of {@link TesLight::ESP32ADC}.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
TesLight::ESP32ADC::ESP32ADC(const uint8_t inputPin, const uint8_t inputMode, const float maxVoltage)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize ESP32ADC."));
	this->inputPin = inputPin;
	this->inputMode = inputMode;
	this->maxVoltage = maxVoltage;
	this->setupPin();
}

/**
 * @brief Delete the {@link TesLight::ESP32ADC} instance and release the pin.
 */
TesLight::ESP32ADC::~ESP32ADC()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete ESP32ADC instance and release the pin."));
	pinMode(this->inputPin, INPUT);
}

/**
 * @brief Set the input pin.
 * @param inputPin physical pin for the input
 */
void TesLight::ESP32ADC::setInputPin(const uint8_t inputPin)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set input pin to ") + inputPin + F("."));
	this->inputPin = inputPin;
	this->setupPin();
}

/**
 * @brief Get the phcysical input pin currently used.
 * @return uint8_t physical pin number
 */
uint8_t TesLight::ESP32ADC::getInputPin()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get input pin."));
	return this->inputPin;
}

/**
 * @brief Set the input mode.
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
void TesLight::ESP32ADC::setInputMode(const uint8_t inputMode)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set input mode to ") + inputMode + F("."));
	this->inputMode = inputMode;
	this->setupPin();
}

/**
 * @brief Get the currently set input mode.
 * @return uint8_t input mode
 */
uint8_t TesLight::ESP32ADC::getInputMode()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get input mode."));
	return this->inputMode;
}

/**
 * @brief Set the maximum voltage value, used to convert from analog value to voltage.
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
void TesLight::ESP32ADC::setMaxVoltage(const float maxVoltage)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set max voltage to ") + maxVoltage + F("V."));
	this->maxVoltage = maxVoltage;
}

/**
 * @brief Get the maximum voltage value, used to convert from analog value to voltage.
 * @return float maximum voltage value, used to convert from analog value to voltage
 */
float TesLight::ESP32ADC::getMaxVoltage()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get max voltage."));
	return this->maxVoltage;
}

/**
 * @brief Get the raw analog value.
 * @return uint16_t raw analog value
 */
uint16_t TesLight::ESP32ADC::getAnalogValue()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read analog value."));
	return analogRead(this->inputPin);
}

/**
 * @brief Get the analog voltage.
 * @param usePolynomialCorrection use a polynomial curve to correct the ADC values
 * @return float analog voltage
 */
float TesLight::ESP32ADC::getAnalogVoltage(const bool usePolynomialCorrection)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read analog voltage."));
	const double analogValue = analogRead(this->inputPin);
	if (analogValue < 1.0f || analogValue > 4095.0f)
	{
		return 0.0f;
	}

	if (usePolynomialCorrection)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Applying polinomial correction."));
		const double correctedVoltage = -0.000000000000016 * pow(analogValue, 4) + 0.000000000118171 * pow(analogValue, 3) - 0.000000301211691 * pow(analogValue, 2) + 0.001109019271794 * analogValue + 0.034143524634089;
		return correctedVoltage / 3.14f * this->maxVoltage;
	}
	else
	{
		return analogValue / 4095.0f * this->maxVoltage;
	}
}

/**
 * @brief Configure the input pin.
 */
void TesLight::ESP32ADC::setupPin()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Setup pin."));
	pinMode(this->inputPin, this->inputMode);
	analogReadResolution(12);
}
