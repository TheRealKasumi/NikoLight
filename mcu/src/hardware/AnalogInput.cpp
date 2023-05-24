/**
 * @file ESP32ADC.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::AnalogInput}.
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

bool NL::AnalogInput::initialized = false;
uint8_t NL::AnalogInput::inputPin;
uint8_t NL::AnalogInput::inputMode;
float NL::AnalogInput::maxVoltage;

/**
 * @brief Start the analog input.
 * @param inputPin physical input pin
 */
void NL::AnalogInput::begin(const uint8_t inputPin)
{
	NL::AnalogInput::begin(inputPin, INPUT, 3.3f);
}

/**
 * @brief Start the analog input.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
void NL::AnalogInput::begin(const uint8_t inputPin, const uint8_t inputMode)
{
	NL::AnalogInput::begin(inputPin, inputMode, 3.3f);
}

/**
 * @brief Start the analog input.
 * @param inputPin physical input pin
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
void NL::AnalogInput::begin(const uint8_t inputPin, const uint8_t inputMode, const float maxVoltage)
{
	NL::AnalogInput::initialized = true;
	NL::AnalogInput::inputPin = inputPin;
	NL::AnalogInput::inputMode = inputMode;
	NL::AnalogInput::maxVoltage = maxVoltage;
	NL::AnalogInput::setupPin();
}

/**
 * @brief Stop the analog input.
 */
void NL::AnalogInput::end()
{
	NL::AnalogInput::initialized = false;
	pinMode(NL::AnalogInput::inputPin, INPUT);
}

/**
 * @brief Return if the sensor was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::AnalogInput::isInitialized()
{
	return NL::AnalogInput::initialized;
}

/**
 * @brief Set the input pin.
 * @param inputPin physical pin for the input
 */
void NL::AnalogInput::setInputPin(const uint8_t inputPin)
{
	NL::AnalogInput::inputPin = inputPin;
	NL::AnalogInput::setupPin();
}

/**
 * @brief Get the phcysical input pin currently used.
 * @return uint8_t physical pin number
 */
uint8_t NL::AnalogInput::getInputPin()
{
	return NL::AnalogInput::inputPin;
}

/**
 * @brief Set the input mode.
 * @param inputMode input mode, INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 */
void NL::AnalogInput::setInputMode(const uint8_t inputMode)
{
	NL::AnalogInput::inputMode = inputMode;
	NL::AnalogInput::setupPin();
}

/**
 * @brief Get the currently set input mode.
 * @return uint8_t input mode
 */
uint8_t NL::AnalogInput::getInputMode()
{
	return NL::AnalogInput::inputMode;
}

/**
 * @brief Set the maximum voltage value, used to convert from analog value to voltage.
 * @param maxVoltage maximum voltage value, used to convert from analog value to voltage
 */
void NL::AnalogInput::setMaxVoltage(const float maxVoltage)
{
	NL::AnalogInput::maxVoltage = maxVoltage;
}

/**
 * @brief Get the maximum voltage value, used to convert from analog value to voltage.
 * @return float maximum voltage value, used to convert from analog value to voltage
 */
float NL::AnalogInput::getMaxVoltage()
{
	return NL::AnalogInput::maxVoltage;
}

/**
 * @brief Get the raw analog value.
 * @return uint16_t raw analog value
 */
uint16_t NL::AnalogInput::getAnalogValue()
{
	return analogRead(NL::AnalogInput::inputPin);
}

/**
 * @brief Get the analog voltage.
 * @param usePolynomialCorrection use a polynomial curve to correct the ADC values
 * @return float analog voltage
 */
float NL::AnalogInput::getAnalogVoltage(const bool usePolynomialCorrection)
{
	const double analogValue = NL::AnalogInput::getAnalogValue();
	if (analogValue < 1.0f || analogValue > 4095.0f)
	{
		return 0.0f;
	}

	if (usePolynomialCorrection)
	{
		const double correctedVoltage = -0.000000000000016 * pow(analogValue, 4) + 0.000000000118171 * pow(analogValue, 3) - 0.000000301211691 * pow(analogValue, 2) + 0.001109019271794 * analogValue + 0.034143524634089;
		return correctedVoltage / 3.14f * NL::AnalogInput::maxVoltage;
	}
	else
	{
		return analogValue / 4095.0f * NL::AnalogInput::maxVoltage;
	}
}

/**
 * @brief Configure the input pin.
 */
void NL::AnalogInput::setupPin()
{
	pinMode(NL::AnalogInput::inputPin, NL::AnalogInput::inputMode);
	analogReadResolution(12);
}
