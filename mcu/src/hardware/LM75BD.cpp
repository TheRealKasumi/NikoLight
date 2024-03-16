/**
 * @file LM75BD.cpp
 * @author TheRealKasumi
 * @brief Implementation of {@link NL::LM75BD}.
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
#include "hardware/LM75BD.h"

#include "configuration/SystemConfiguration.h"

#if defined(HW_VERSION_2_2)

void IRAM_ATTR (*NL::LM75BD::alarmCallback)() = nullptr;

/**
 * @brief Create a new instance of {@link NL::LM75BD}.
 * @param deviceAddress I²C address of the device
 * @param interruptPin optional pin for the over temperature interrupt, disabled when not set
 */
NL::LM75BD::LM75BD(const uint8_t deviceAddress, const uint8_t interruptPin)
{
	// Set the the I²C address and pin
	this->deviceAddress = deviceAddress;
	this->interruptPin = interruptPin;

	// Create a default configuration
	NL::LM75BD::Configuration configuration;
	configuration.overTemperatureFaultQueue = NL::LM75BD::OTFaultQueue::QUEUE_6;
	configuration.overTemperaturePolarity = NL::LM75BD::OTPolarity::ACTIVE_LOW;
	configuration.overTemperatureOperationMode = NL::LM75BD::OTOperationMode::COMP;
	configuration.powerState = NL::LM75BD::PowerState::ACTIVE;

	// Configure the interrupt when enabled
	if (this->interruptPin != 0)
	{
		// Configure the pin
		pinMode(this->interruptPin, INPUT_PULLUP);

		// Attach the interrupt
		attachInterrupt(digitalPinToInterrupt(this->interruptPin), NL::LM75BD::alarmInterruptHandler, FALLING);
	}

	// Write the default configuration
	this->setConfiguration(configuration);
}

/**
 * @brief Destroy the {NL::LM75BD} instance.
 */
NL::LM75BD::~LM75BD()
{
	// Detach the interrupt
	if (this->interruptPin != 0)
	{
		pinMode(this->interruptPin, INPUT);
		detachInterrupt(digitalPinToInterrupt(this->interruptPin));
	}
}

/**
 * @brief Set the configuration.
 * @param configuration configuration of the sensor
 */
void NL::LM75BD::setConfiguration(const NL::LM75BD::Configuration configuration)
{
	// Prepare the configuration byte
	uint8_t rawConfiguration = 0;
	rawConfiguration |= static_cast<uint8_t>(configuration.overTemperatureFaultQueue);
	rawConfiguration |= static_cast<uint8_t>(configuration.overTemperatureFaultQueue);
	rawConfiguration |= static_cast<uint8_t>(configuration.overTemperatureFaultQueue);
	rawConfiguration |= static_cast<uint8_t>(configuration.overTemperatureFaultQueue);

	// Start the transmission and set the pointer to the configuration register
	Wire.beginTransmission(this->deviceAddress);
	if (Wire.write(0x01) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set configuration of the LM75BD. Could not write register address.", SOURCE_LOCATION);
		return;
	}

	// Write the configuration byte
	if (Wire.write(rawConfiguration) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set configuration of the LM75BD. Could not write configuration registers.", SOURCE_LOCATION);
		return;
	}

	// End the transmission and check for errors
	if (Wire.endTransmission(true) != 0)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set configuration of the LM75BD. Error in I²C communication.", SOURCE_LOCATION);
		return;
	}
}

/**
 * @brief Get the configuration.
 * @return configuration of the sensor
 */
const NL::LM75BD::Configuration NL::LM75BD::getConfiguration()
{
	// Start the transmission and set the pointer to the configuration register
	Wire.beginTransmission(this->deviceAddress);
	if (Wire.write(0x01) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get configuration of the LM75BD. Could not write register address.", SOURCE_LOCATION);
		return NL::LM75BD::Configuration();
	}

	// End the transmission and check for errors
	if (Wire.endTransmission(true) != 0)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get configuration of the LM75BD. Error in I²C communication.", SOURCE_LOCATION);
		return NL::LM75BD::Configuration();
	}

	// Request the configuration register
	if (Wire.requestFrom(static_cast<int>(this->deviceAddress), 1, static_cast<int>(true)) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get configuration of the LM75BD. Could not receive configuration registers.", SOURCE_LOCATION);
		return NL::LM75BD::Configuration();
	}

	// Read and return the configuration
	const uint8_t rawConfiguration = Wire.read();
	NL::LM75BD::Configuration configuration;
	configuration.overTemperatureFaultQueue = static_cast<NL::LM75BD::OTFaultQueue>(rawConfiguration & 0b00011000);
	configuration.overTemperaturePolarity = static_cast<NL::LM75BD::OTPolarity>(rawConfiguration & 0b00000100);
	configuration.overTemperatureOperationMode = static_cast<NL::LM75BD::OTOperationMode>(rawConfiguration & 0b00000010);
	configuration.powerState = static_cast<NL::LM75BD::PowerState>(rawConfiguration & 0b00000001);
	return configuration;
}

/**
 * @brief Set the over temperature threshold in °C.
 * @param threshold threshold in °C
 * @throws {@link NL::ConstraintViolationException} when the threshold is invalid
 */
void NL::LM75BD::setOverTemperatureThreshold(float threshold)
{
	// Make sure the threshold is within the valid range
	if (threshold > 127.0f || threshold < -128.0f)
	{
		// Workaround for v2.2
		// throw NL::ConstraintViolationException("The threshold must be between -128°C and 127°C.", SOURCE_LOCATION);
		return;
	}

	// Prepare the temperature bytes
	const int16_t rawTemperature = static_cast<int16_t>(threshold * 2.0f * 128.0f);

	// Start the transmission and set the pointer to the threshold register
	Wire.beginTransmission(this->deviceAddress);
	if (Wire.write(0x03) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set temperature threshold of the LM75BD. Could not write register address.", SOURCE_LOCATION);
		return;
	}

	// Write the two temperature bytes
	const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&rawTemperature);
	if (Wire.write(bytes[1]) != 1 || Wire.write(bytes[0]) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set temperature threshold of the LM75BD. Could not write temperature registers.", SOURCE_LOCATION);
		return;
	}

	// End the transmission and check for errors
	if (Wire.endTransmission(true) != 0)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set temperature threshold of the LM75BD. Error in I²C communication.", SOURCE_LOCATION);
		return;
	}
}

/**
 * @brief Get the over temperature threshold in °C.
 * @return threshold in °C
 */
const float NL::LM75BD::getOverTemperatureThreshold()
{
	// Start the transmission and set the pointer to the threshold register
	Wire.beginTransmission(this->deviceAddress);
	if (Wire.write(0x03) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature threshold of the LM75BD. Could not write register address.", SOURCE_LOCATION);
		return 0.0f;
	}

	// End the transmission and check for errors
	if (Wire.endTransmission(true) != 0)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature threshold of the LM75BD. Error in I²C communication.", SOURCE_LOCATION);
		return 0.0f;
	}

	// Request the two threshold register
	if (Wire.requestFrom(static_cast<int>(this->deviceAddress), 2, static_cast<int>(true)) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature threshold of the LM75BD. Could not receive registers.", SOURCE_LOCATION);
		return 0.0f;
	}

	// Read the two temperature bytes
	int16_t rawTemperature = Wire.read() << 8;
	rawTemperature |= Wire.read();
	rawTemperature = rawTemperature / 128;

	// Convert the raw value to °C and return it
	return rawTemperature * 0.5f;
}

/**
 * @brief Set the over temperature hysteresis in °C.
 * @param hysteresis hysteresis in °C
 * @throws {@link NL::ConstraintViolationException} when the threshold is invalid
 */
void NL::LM75BD::setOverTemperatureHysteresis(float hysteresis)
{
	// Make sure the hysteresis is within the valid range
	if (hysteresis > 127.0f || hysteresis < -128.0f)
	{
		// Workaround for v2.2
		// throw NL::ConstraintViolationException("The hysteresis must be between -128°C and 127°C.", SOURCE_LOCATION);
		return;
	}

	// Prepare the temperature bytes
	const int16_t rawTemperature = static_cast<int16_t>(hysteresis * 2.0f * 128.0f);

	// Start the transmission and set the pointer to the hysteresis register
	Wire.beginTransmission(this->deviceAddress);
	if (Wire.write(0x03) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set temperature hysteresis of the LM75BD. Could not write register address.", SOURCE_LOCATION);
		return;
	}

	// Write the two temperature bytes
	const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&rawTemperature);
	if (Wire.write(bytes[1]) != 1 || Wire.write(bytes[0]) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set temperature hysteresis of the LM75BD. Could not write temperature registers.", SOURCE_LOCATION);
		return;
	}

	// End the transmission and check for errors
	if (Wire.endTransmission(true) != 0)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to set temperature hysteresis of the LM75BD. Error in I²C communication.", SOURCE_LOCATION);
		return;
	}
}

/**
 * @brief Get the over temperature hysteresis in °C.
 * @return hysteresis in °C
 */
const float NL::LM75BD::getOverTemperatureHysteresis()
{
	// Start the transmission and set the pointer to the hysteresis register
	Wire.beginTransmission(this->deviceAddress);
	if (Wire.write(0x03) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature hysteresis of the LM75BD. Could not write register address.", SOURCE_LOCATION);
		return 0.0f;
	}

	// End the transmission and check for errors
	if (Wire.endTransmission(true) != 0)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature hysteresis of the LM75BD. Error in I²C communication.", SOURCE_LOCATION);
		return 0.0f;
	}

	// Request the two hysteresis register
	if (Wire.requestFrom(static_cast<int>(this->deviceAddress), 2, static_cast<int>(true)) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature hysteresis of the LM75BD. Could not receive temperature registers.", SOURCE_LOCATION);
		return 0.0f;
	}

	// Read the two temperature bytes
	int16_t rawTemperature = Wire.read() << 8;
	rawTemperature |= Wire.read();
	rawTemperature = rawTemperature / 128;

	// Convert the raw value to °C and return it
	return rawTemperature * 0.5f;
}

/**
 * @brief Set the over temperature callback.
 * @param alarmCallback function to call in case of over temperature events
 */
void NL::LM75BD::setAlarmCallback(void (*alarmCallback)())
{
	NL::LM75BD::alarmCallback = alarmCallback;
}

/**
 * @brief Get the temperature of the sensor in °C.
 * @return temperature of the sensor in °C
 */
const float NL::LM75BD::getTemperature()
{
	// Start the transmission and set the pointer to the temperature register
	Wire.beginTransmission(this->deviceAddress);
	if (Wire.write(0x00) != 1)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature value of the LM75BD. Could not write register address.", SOURCE_LOCATION);
		return 0.0f;
	}

	// End the transmission and check for errors
	if (Wire.endTransmission(true) != 0)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature value of the LM75BD. Error in I²C communication.", SOURCE_LOCATION);
		return 0.0f;
	}

	// Request the two temperature register
	if (Wire.requestFrom(static_cast<int>(this->deviceAddress), 2, static_cast<int>(true)) != 2)
	{
		// Workaround for v2.2
		// throw NL::IICException("Failed to get temperature value of the LM75BD. Could not receive temperature registers.", SOURCE_LOCATION);
		return 0.0f;
	}

	// Read the two temperature bytes
	int16_t rawTemperature = Wire.read() << 8;
	rawTemperature |= Wire.read();
	rawTemperature = rawTemperature / 32;

	// Convert the raw value to °C and return it
	return rawTemperature * 0.125f;
}

/**
 * @brief Interrupt handler for temperature alarm interrupts.
 */
void IRAM_ATTR NL::LM75BD::alarmInterruptHandler()
{
	if (NL::LM75BD::alarmCallback)
	{
		NL::LM75BD::alarmCallback();
	}
}

#endif
