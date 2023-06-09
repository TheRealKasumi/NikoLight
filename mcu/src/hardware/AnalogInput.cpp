/**
 * @file ESP32ADC.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::AnalogInput}.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
