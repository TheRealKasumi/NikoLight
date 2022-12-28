/**
 * @file ESP32ADC.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::ESP32ADC}.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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
#include "hardware/ESP32ADC.h"

/**
 * @brief Create a new instance of {@link TL::ESP32ADC}.
 * @param inputPin physical input pin
 */
TL::ESP32ADC::ESP32ADC(const uint8_t inputPin)
{
	this->inputPin = inputPin;
	this->inputMode = INPUT;
	this->maxVoltage = 3.3f;
	this->setupPin();
}

/**
 * @brief Create a new instance of {@link TL::ESP32ADC}.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
TL::ESP32ADC::ESP32ADC(const uint8_t inputPin, const uint8_t inputMode)
{
	this->inputPin = inputPin;
	this->inputMode = inputMode;
	this->maxVoltage = 3.3f;
	this->setupPin();
}

/**
 * @brief Create a new instance of {@link TL::ESP32ADC}.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
TL::ESP32ADC::ESP32ADC(const uint8_t inputPin, const uint8_t inputMode, const float maxVoltage)
{
	this->inputPin = inputPin;
	this->inputMode = inputMode;
	this->maxVoltage = maxVoltage;
	this->setupPin();
}

/**
 * @brief Delete the {@link TL::ESP32ADC} instance and release the pin.
 */
TL::ESP32ADC::~ESP32ADC()
{
	pinMode(this->inputPin, INPUT);
}

/**
 * @brief Set the input pin.
 * @param inputPin physical pin for the input
 */
void TL::ESP32ADC::setInputPin(const uint8_t inputPin)
{
	this->inputPin = inputPin;
	this->setupPin();
}

/**
 * @brief Get the phcysical input pin currently used.
 * @return uint8_t physical pin number
 */
uint8_t TL::ESP32ADC::getInputPin()
{
	return this->inputPin;
}

/**
 * @brief Set the input mode.
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
void TL::ESP32ADC::setInputMode(const uint8_t inputMode)
{
	this->inputMode = inputMode;
	this->setupPin();
}

/**
 * @brief Get the currently set input mode.
 * @return uint8_t input mode
 */
uint8_t TL::ESP32ADC::getInputMode()
{
	return this->inputMode;
}

/**
 * @brief Set the maximum voltage value, used to convert from analog value to voltage.
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
void TL::ESP32ADC::setMaxVoltage(const float maxVoltage)
{
	this->maxVoltage = maxVoltage;
}

/**
 * @brief Get the maximum voltage value, used to convert from analog value to voltage.
 * @return float maximum voltage value, used to convert from analog value to voltage
 */
float TL::ESP32ADC::getMaxVoltage()
{
	return this->maxVoltage;
}

/**
 * @brief Get the raw analog value.
 * @return uint16_t raw analog value
 */
uint16_t TL::ESP32ADC::getAnalogValue()
{
	return analogRead(this->inputPin);
}

/**
 * @brief Get the analog voltage.
 * @param usePolynomialCorrection use a polynomial curve to correct the ADC values
 * @return float analog voltage
 */
float TL::ESP32ADC::getAnalogVoltage(const bool usePolynomialCorrection)
{
	const double analogValue = this->getAnalogValue();
	if (analogValue < 1.0f || analogValue > 4095.0f)
	{
		return 0.0f;
	}

	if (usePolynomialCorrection)
	{
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
void TL::ESP32ADC::setupPin()
{
	pinMode(this->inputPin, this->inputMode);
	analogReadResolution(12);
}
