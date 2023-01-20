/**
 * @file ESP32ADC.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::AnalogInput}.
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
#include "hardware/AnalogInput.h"

bool TL::AnalogInput::initialized = false;
uint8_t TL::AnalogInput::inputPin;
uint8_t TL::AnalogInput::inputMode;
float TL::AnalogInput::maxVoltage;

/**
 * @brief Start the analog input.
 * @param inputPin physical input pin
 */
void TL::AnalogInput::begin(const uint8_t inputPin)
{
	TL::AnalogInput::begin(inputPin, INPUT, 3.3f);
}

/**
 * @brief Start the analog input.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
void TL::AnalogInput::begin(const uint8_t inputPin, const uint8_t inputMode)
{
	TL::AnalogInput::begin(inputPin, inputMode, 3.3f);
}

/**
 * @brief Start the analog input.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
void TL::AnalogInput::begin(const uint8_t inputPin, const uint8_t inputMode, const float maxVoltage)
{
	TL::AnalogInput::initialized = true;
	TL::AnalogInput::inputPin = inputPin;
	TL::AnalogInput::inputMode = inputMode;
	TL::AnalogInput::maxVoltage = maxVoltage;
	TL::AnalogInput::setupPin();
}

/**
 * @brief Stop the analog input.
 */
void TL::AnalogInput::end()
{
	TL::AnalogInput::initialized = false;
	pinMode(TL::AnalogInput::inputPin, INPUT);
}

/**
 * @brief Return if the sensor was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::AnalogInput::isInitialized()
{
	return TL::AnalogInput::initialized;
}

/**
 * @brief Set the input pin.
 * @param inputPin physical pin for the input
 */
void TL::AnalogInput::setInputPin(const uint8_t inputPin)
{
	TL::AnalogInput::inputPin = inputPin;
	TL::AnalogInput::setupPin();
}

/**
 * @brief Get the phcysical input pin currently used.
 * @return uint8_t physical pin number
 */
uint8_t TL::AnalogInput::getInputPin()
{
	return TL::AnalogInput::inputPin;
}

/**
 * @brief Set the input mode.
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
void TL::AnalogInput::setInputMode(const uint8_t inputMode)
{
	TL::AnalogInput::inputMode = inputMode;
	TL::AnalogInput::setupPin();
}

/**
 * @brief Get the currently set input mode.
 * @return uint8_t input mode
 */
uint8_t TL::AnalogInput::getInputMode()
{
	return TL::AnalogInput::inputMode;
}

/**
 * @brief Set the maximum voltage value, used to convert from analog value to voltage.
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
void TL::AnalogInput::setMaxVoltage(const float maxVoltage)
{
	TL::AnalogInput::maxVoltage = maxVoltage;
}

/**
 * @brief Get the maximum voltage value, used to convert from analog value to voltage.
 * @return float maximum voltage value, used to convert from analog value to voltage
 */
float TL::AnalogInput::getMaxVoltage()
{
	return TL::AnalogInput::maxVoltage;
}

/**
 * @brief Get the raw analog value.
 * @return uint16_t raw analog value
 */
uint16_t TL::AnalogInput::getAnalogValue()
{
	return analogRead(TL::AnalogInput::inputPin);
}

/**
 * @brief Get the analog voltage.
 * @param usePolynomialCorrection use a polynomial curve to correct the ADC values
 * @return float analog voltage
 */
float TL::AnalogInput::getAnalogVoltage(const bool usePolynomialCorrection)
{
	const double analogValue = TL::AnalogInput::getAnalogValue();
	if (analogValue < 1.0f || analogValue > 4095.0f)
	{
		return 0.0f;
	}

	if (usePolynomialCorrection)
	{
		const double correctedVoltage = -0.000000000000016 * pow(analogValue, 4) + 0.000000000118171 * pow(analogValue, 3) - 0.000000301211691 * pow(analogValue, 2) + 0.001109019271794 * analogValue + 0.034143524634089;
		return correctedVoltage / 3.14f * TL::AnalogInput::maxVoltage;
	}
	else
	{
		return analogValue / 4095.0f * TL::AnalogInput::maxVoltage;
	}
}

/**
 * @brief Configure the input pin.
 */
void TL::AnalogInput::setupPin()
{
	pinMode(TL::AnalogInput::inputPin, TL::AnalogInput::inputMode);
	analogReadResolution(12);
}
