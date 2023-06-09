/**
 * @file AudioUnit.cpp
 * @author TheRealKasumi
 * @brief Implementation of the NikoLight Audio Unit driver.
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
#include "hardware/AudioUnit.h"

bool NL::AudioUnit::initialized = false;
uint8_t NL::AudioUnit::deviceAddress;
uint8_t NL::AudioUnit::deviceFunction;
uint8_t NL::AudioUnit::frequencyBandCount;

/**
 * @brief Start the audio unit.
 * @return OK when the audio unit was started
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 */
NL::AudioUnit::Error NL::AudioUnit::begin(const uint8_t deviceAddress)
{
	NL::AudioUnit::initialized = false;
	NL::AudioUnit::deviceAddress = deviceAddress;
	NL::AudioUnit::deviceFunction = 0;
	NL::AudioUnit::frequencyBandCount = 0;

	Wire.beginTransmission(NL::AudioUnit::deviceAddress);
	Wire.write(NL::AudioUnit::deviceFunction);
	Wire.write(0);
	if (Wire.endTransmission(true) != 0)
	{
		return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}
	Wire.flush();

	NL::AudioUnit::deviceFunction = 1;
	Wire.beginTransmission(NL::AudioUnit::deviceAddress);
	Wire.write(NL::AudioUnit::deviceFunction);
	Wire.write(0);
	if (Wire.endTransmission(true) != 0)
	{
		return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	const uint8_t receivedLength = Wire.requestFrom(static_cast<int>(NL::AudioUnit::deviceAddress), static_cast<int>(1));
	if (receivedLength != 1)
	{
		return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}
	Wire.readBytes(static_cast<uint8_t *>(&NL::AudioUnit::frequencyBandCount), sizeof(NL::AudioUnit::frequencyBandCount));

	NL::AudioUnit::initialized = true;
	return NL::AudioUnit::Error::OK;
}

/**
 * @brief Check if the audio unit was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::AudioUnit::isInitialized()
{
	return NL::AudioUnit::initialized;
}

/**
 * @brief Stop the audio unit.
 */
void NL::AudioUnit::end()
{
	NL::AudioUnit::initialized = false;
}

/**
 * @brief Get the number of frequency bands for which the unit is configured.
 * @return number of frequency band for which the unit is configured
 */
uint8_t NL::AudioUnit::getFrequencyBandCount()
{
	return NL::AudioUnit::frequencyBandCount;
}

/**
 * @brief Get the audio analysis from the audio unit.
 * @param audioAnalysis reference to a variable holding the audio analysis
 * @return OK when the data was read
 * @return ERROR_IIC_COMMUNICATION when there was a communication error
 */
NL::AudioUnit::Error NL::AudioUnit::getAudioAnalysis(NL::AudioUnit::AudioAnalysis &audioAnalysis)
{
	if (NL::AudioUnit::deviceFunction != 4)
	{
		NL::AudioUnit::deviceFunction = 4;
		Wire.beginTransmission(NL::AudioUnit::deviceAddress);
		Wire.write(NL::AudioUnit::deviceFunction);
		Wire.write(0);
		if (Wire.endTransmission(true) != 0)
		{
			return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
		}
	}

	const uint8_t expectedSize = 3 + 2 * NL::AudioUnit::frequencyBandCount + 9 * NL::AudioUnit::frequencyBandCount;
	const uint8_t receivedLength = Wire.requestFrom(NL::AudioUnit::deviceAddress, expectedSize);
	if (receivedLength != expectedSize)
	{
		return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	audioAnalysis.frequencyBandValues.resize(NL::AudioUnit::frequencyBandCount);
	Wire.readBytes(reinterpret_cast<uint8_t *>(&audioAnalysis.seq), sizeof(audioAnalysis.seq));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&audioAnalysis.volumePeak), sizeof(audioAnalysis.volumePeak));
	Wire.readBytes(reinterpret_cast<uint8_t *>(audioAnalysis.frequencyBandValues.data()), audioAnalysis.frequencyBandValues.size() * sizeof(uint16_t));

	audioAnalysis.frequencyBandTriggers.resize(NL::AudioUnit::frequencyBandCount);
	for (uint8_t i = 0; i < NL::AudioUnit::frequencyBandCount; i++)
	{
		uint8_t trigger;
		uint16_t values[4];
		Wire.readBytes(reinterpret_cast<uint8_t *>(&trigger), sizeof(trigger));
		Wire.readBytes(reinterpret_cast<uint8_t *>(&values), sizeof(values));
		audioAnalysis.frequencyBandTriggers.at(i).trigger = (NL::AudioUnit::Trigger)trigger;
		audioAnalysis.frequencyBandTriggers.at(i).value = values[0];
		audioAnalysis.frequencyBandTriggers.at(i).mean = values[1];
		audioAnalysis.frequencyBandTriggers.at(i).standardDeviation = values[2];
		audioAnalysis.frequencyBandTriggers.at(i).triggerThreshold = values[3];
	}

	return NL::AudioUnit::Error::OK;
}

/**
 * @brief Get the audio unit configuration.
 * @param unitConfig reference to a variable holding the unit config
 * @return OK when the configuration was read
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 */
NL::AudioUnit::Error NL::AudioUnit::getAudioUnitConfig(NL::AudioUnit::AudioUnitConfig &unitConfig)
{
	if (NL::AudioUnit::deviceFunction != 2)
	{
		NL::AudioUnit::deviceFunction = 2;
		Wire.beginTransmission(NL::AudioUnit::deviceAddress);
		Wire.write(NL::AudioUnit::deviceFunction);
		Wire.write(0);
		if (Wire.endTransmission(true) != 0)
		{
			return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
		}
	}

	const uint8_t expectedSize = 4 * NL::AudioUnit::frequencyBandCount + 2;
	const uint8_t receivedLength = Wire.requestFrom(NL::AudioUnit::deviceAddress, expectedSize);
	if (receivedLength != expectedSize)
	{
		return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	Wire.readBytes(reinterpret_cast<uint8_t *>(&unitConfig.noiseThreshold), sizeof(unitConfig.noiseThreshold));
	unitConfig.frequencyBandIndex.resize(NL::AudioUnit::frequencyBandCount);
	for (uint8_t i = 0; i < NL::AudioUnit::frequencyBandCount; i++)
	{
		uint16_t range[2];
		Wire.readBytes(reinterpret_cast<uint8_t *>(range), sizeof(range));
		unitConfig.frequencyBandIndex.at(i) = std::make_pair(range[0], range[1]);
	}

	return NL::AudioUnit::Error::OK;
}

/**
 * @brief Set the audio unit configuration.
 * @param unitConfig new configuration to set
 * @return OK when the new configuration was set
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 * @return ERROR_INVALID_ARGUMENT when the configuration is invalid
 */
NL::AudioUnit::Error NL::AudioUnit::setAudioUnitConfig(NL::AudioUnit::AudioUnitConfig &unitConfig)
{
	if (unitConfig.frequencyBandIndex.size() > NL::AudioUnit::frequencyBandCount)
	{
		return NL::AudioUnit::Error::ERROR_INVALID_ARGUMENT;
	}

	NL::AudioUnit::deviceFunction = 100;
	Wire.beginTransmission(NL::AudioUnit::deviceAddress);
	Wire.write(NL::AudioUnit::deviceFunction);
	Wire.write(0);
	Wire.write(reinterpret_cast<uint8_t *>(&unitConfig.noiseThreshold), sizeof(unitConfig.noiseThreshold));
	for (uint8_t i = 0; i < NL::AudioUnit::frequencyBandCount; i++)
	{
		uint16_t range[2] = {
			unitConfig.frequencyBandIndex.at(i).first,
			unitConfig.frequencyBandIndex.at(i).second};
		Wire.write(reinterpret_cast<uint8_t *>(range), sizeof(range));
	}

	return Wire.endTransmission(true) == 0 ? NL::AudioUnit::Error::OK : NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
}

/**
 * @brief Get the peak detector configuration.
 * @param peakDetectorConfig reference to a variable holding the configuration
 * @param index index of the configuration
 * @return OK when the configuration was read
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 */
NL::AudioUnit::Error NL::AudioUnit::getPeakDetectorConfig(NL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index)
{
	if (NL::AudioUnit::deviceFunction != 3)
	{
		NL::AudioUnit::deviceFunction = 3;
		Wire.beginTransmission(NL::AudioUnit::deviceAddress);
		Wire.write(NL::AudioUnit::deviceFunction);
		Wire.write(index);
		if (Wire.endTransmission(true) != 0)
		{
			return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
		}
	}

	const uint8_t expectedSize = 26;
	const uint8_t receivedLength = Wire.requestFrom(NL::AudioUnit::deviceAddress, expectedSize);
	if (receivedLength != expectedSize)
	{
		return NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.historySize), sizeof(peakDetectorConfig.historySize));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.threshold), sizeof(peakDetectorConfig.threshold));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.influence), sizeof(peakDetectorConfig.influence));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.noiseGate), sizeof(peakDetectorConfig.noiseGate));

	return NL::AudioUnit::Error::OK;
}

/**
 * @brief Set the peak detector configuration.
 * @param peakDetectorConfig new peak detector configuration to set
 * @param index index of the configuration
 * @return OK when the new configuration was set
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 * @return ERROR_INVALID_ARGUMENT when the configuration is invalid
 */
NL::AudioUnit::Error NL::AudioUnit::setPeakDetectorConfig(NL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index)
{
	if (index >= NL::AudioUnit::frequencyBandCount)
	{
		return NL::AudioUnit::Error::ERROR_INVALID_ARGUMENT;
	}

	NL::AudioUnit::deviceFunction = 101;
	Wire.beginTransmission(NL::AudioUnit::deviceAddress);
	Wire.write(NL::AudioUnit::deviceFunction);
	Wire.write(index);
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.historySize), sizeof(peakDetectorConfig.historySize));
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.threshold), sizeof(peakDetectorConfig.threshold));
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.influence), sizeof(peakDetectorConfig.influence));
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.noiseGate), sizeof(peakDetectorConfig.noiseGate));

	return Wire.endTransmission(true) == 0 ? NL::AudioUnit::Error::OK : NL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
}
