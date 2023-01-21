/**
 * @file AudioUnit.cpp
 * @author TheRealKasumi
 * @brief Implementation of the TesLight Audio Unit driver.
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
#include "hardware/AudioUnit.h"

bool TL::AudioUnit::initialized = false;
uint8_t TL::AudioUnit::deviceAddress;
uint8_t TL::AudioUnit::deviceFunction;
uint8_t TL::AudioUnit::frequencyBandCount;

/**
 * @brief Start the audio unit.
 * @return OK when the audio unit was started
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 */
TL::AudioUnit::Error TL::AudioUnit::begin(const uint8_t deviceAddress)
{
	TL::AudioUnit::initialized = false;
	TL::AudioUnit::deviceAddress = deviceAddress;
	TL::AudioUnit::deviceFunction = 0;
	TL::AudioUnit::frequencyBandCount = 0;

	Wire.beginTransmission(TL::AudioUnit::deviceAddress);
	Wire.write(TL::AudioUnit::deviceFunction);
	Wire.write(0);
	if (Wire.endTransmission(true) != 0)
	{
		return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}
	Wire.flush();

	TL::AudioUnit::deviceFunction = 1;
	Wire.beginTransmission(TL::AudioUnit::deviceAddress);
	Wire.write(TL::AudioUnit::deviceFunction);
	Wire.write(0);
	if (Wire.endTransmission(true) != 0)
	{
		return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	const uint8_t receivedLength = Wire.requestFrom(static_cast<int>(TL::AudioUnit::deviceAddress), static_cast<int>(1));
	if (receivedLength != 1)
	{
		return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}
	Wire.readBytes(static_cast<uint8_t *>(&TL::AudioUnit::frequencyBandCount), sizeof(TL::AudioUnit::frequencyBandCount));

	TL::AudioUnit::initialized = true;
	return TL::AudioUnit::Error::OK;
}

/**
 * @brief Check if the audio unit was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::AudioUnit::isInitialized()
{
	return TL::AudioUnit::initialized;
}

/**
 * @brief Stop the audio unit.
 */
void TL::AudioUnit::end()
{
	TL::AudioUnit::initialized = false;
}

/**
 * @brief Get the number of frequency bands for which the unit is configured.
 * @return number of frequency band for which the unit is configured
 */
uint8_t TL::AudioUnit::getFrequencyBandCount()
{
	return TL::AudioUnit::frequencyBandCount;
}

/**
 * @brief Get the audio analysis from the audio unit.
 * @param audioAnalysis reference to a variable holding the audio analysis
 * @return OK when the data was read
 * @return ERROR_IIC_COMMUNICATION when there was a communication error
 */
TL::AudioUnit::Error TL::AudioUnit::getAudioAnalysis(TL::AudioUnit::AudioAnalysis &audioAnalysis)
{
	if (TL::AudioUnit::deviceFunction != 4)
	{
		TL::AudioUnit::deviceFunction = 4;
		Wire.beginTransmission(TL::AudioUnit::deviceAddress);
		Wire.write(TL::AudioUnit::deviceFunction);
		Wire.write(0);
		if (Wire.endTransmission(true) != 0)
		{
			return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
		}
	}

	const uint8_t expectedSize = 3 + 2 * TL::AudioUnit::frequencyBandCount + 9 * TL::AudioUnit::frequencyBandCount;
	const uint8_t receivedLength = Wire.requestFrom(TL::AudioUnit::deviceAddress, expectedSize);
	if (receivedLength != expectedSize)
	{
		return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	audioAnalysis.frequencyBandValues.resize(TL::AudioUnit::frequencyBandCount);
	Wire.readBytes(reinterpret_cast<uint8_t *>(&audioAnalysis.seq), sizeof(audioAnalysis.seq));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&audioAnalysis.volumePeak), sizeof(audioAnalysis.volumePeak));
	Wire.readBytes(reinterpret_cast<uint8_t *>(audioAnalysis.frequencyBandValues.data()), audioAnalysis.frequencyBandValues.size() * sizeof(uint16_t));

	audioAnalysis.frequencyBandTriggers.resize(TL::AudioUnit::frequencyBandCount);
	for (uint8_t i = 0; i < TL::AudioUnit::frequencyBandCount; i++)
	{
		uint8_t trigger;
		uint16_t values[4];
		Wire.readBytes(reinterpret_cast<uint8_t *>(&trigger), sizeof(trigger));
		Wire.readBytes(reinterpret_cast<uint8_t *>(&values), sizeof(values));
		audioAnalysis.frequencyBandTriggers.at(i).trigger = (TL::AudioUnit::Trigger)trigger;
		audioAnalysis.frequencyBandTriggers.at(i).value = values[0];
		audioAnalysis.frequencyBandTriggers.at(i).mean = values[1];
		audioAnalysis.frequencyBandTriggers.at(i).standardDeviation = values[2];
		audioAnalysis.frequencyBandTriggers.at(i).triggerThreshold = values[3];
	}

	return TL::AudioUnit::Error::OK;
}

/**
 * @brief Get the audio unit configuration.
 * @param unitConfig reference to a variable holding the unit config
 * @return OK when the configuration was read
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 */
TL::AudioUnit::Error TL::AudioUnit::getAudioUnitConfig(TL::AudioUnit::AudioUnitConfig &unitConfig)
{
	if (TL::AudioUnit::deviceFunction != 2)
	{
		TL::AudioUnit::deviceFunction = 2;
		Wire.beginTransmission(TL::AudioUnit::deviceAddress);
		Wire.write(TL::AudioUnit::deviceFunction);
		Wire.write(0);
		if (Wire.endTransmission(true) != 0)
		{
			return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
		}
	}

	const uint8_t expectedSize = 4 * TL::AudioUnit::frequencyBandCount + 2;
	const uint8_t receivedLength = Wire.requestFrom(TL::AudioUnit::deviceAddress, expectedSize);
	if (receivedLength != expectedSize)
	{
		return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	Wire.readBytes(reinterpret_cast<uint8_t *>(&unitConfig.noiseThreshold), sizeof(unitConfig.noiseThreshold));
	unitConfig.frequencyBandIndex.resize(TL::AudioUnit::frequencyBandCount);
	for (uint8_t i = 0; i < TL::AudioUnit::frequencyBandCount; i++)
	{
		uint16_t range[2];
		Wire.readBytes(reinterpret_cast<uint8_t *>(range), sizeof(range));
		unitConfig.frequencyBandIndex.at(i) = std::make_pair(range[0], range[1]);
	}

	return TL::AudioUnit::Error::OK;
}

/**
 * @brief Set the audio unit configuration.
 * @param unitConfig new configuration to set
 * @return OK when the new configuration was set
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 * @return ERROR_INVALID_ARGUMENT when the configuration is invalid
 */
TL::AudioUnit::Error TL::AudioUnit::setAudioUnitConfig(TL::AudioUnit::AudioUnitConfig &unitConfig)
{
	if (unitConfig.frequencyBandIndex.size() > TL::AudioUnit::frequencyBandCount)
	{
		return TL::AudioUnit::Error::ERROR_INVALID_ARGUMENT;
	}

	TL::AudioUnit::deviceFunction = 100;
	Wire.beginTransmission(TL::AudioUnit::deviceAddress);
	Wire.write(TL::AudioUnit::deviceFunction);
	Wire.write(0);
	Wire.write(reinterpret_cast<uint8_t *>(&unitConfig.noiseThreshold), sizeof(unitConfig.noiseThreshold));
	for (uint8_t i = 0; i < TL::AudioUnit::frequencyBandCount; i++)
	{
		uint16_t range[2] = {
			unitConfig.frequencyBandIndex.at(i).first,
			unitConfig.frequencyBandIndex.at(i).second};
		Wire.write(reinterpret_cast<uint8_t *>(range), sizeof(range));
	}

	return Wire.endTransmission(true) == 0 ? TL::AudioUnit::Error::OK : TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
}

/**
 * @brief Get the peak detector configuration.
 * @param peakDetectorConfig reference to a variable holding the configuration
 * @param index index of the configuration
 * @return OK when the configuration was read
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 */
TL::AudioUnit::Error TL::AudioUnit::getPeakDetectorConfig(TL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index)
{
	if (TL::AudioUnit::deviceFunction != 3)
	{
		TL::AudioUnit::deviceFunction = 3;
		Wire.beginTransmission(TL::AudioUnit::deviceAddress);
		Wire.write(TL::AudioUnit::deviceFunction);
		Wire.write(index);
		if (Wire.endTransmission(true) != 0)
		{
			return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
		}
	}

	const uint8_t expectedSize = 26;
	const uint8_t receivedLength = Wire.requestFrom(TL::AudioUnit::deviceAddress, expectedSize);
	if (receivedLength != expectedSize)
	{
		return TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
	}

	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.historySize), sizeof(peakDetectorConfig.historySize));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.threshold), sizeof(peakDetectorConfig.threshold));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.influence), sizeof(peakDetectorConfig.influence));
	Wire.readBytes(reinterpret_cast<uint8_t *>(&peakDetectorConfig.noiseGate), sizeof(peakDetectorConfig.noiseGate));

	return TL::AudioUnit::Error::OK;
}

/**
 * @brief Set the peak detector configuration.
 * @param peakDetectorConfig new peak detector configuration to set
 * @param index index of the configuration
 * @return OK when the new configuration was set
 * @return ERROR_IIC_COMMUNICATION when the communication failed
 * @return ERROR_INVALID_ARGUMENT when the configuration is invalid
 */
TL::AudioUnit::Error TL::AudioUnit::setPeakDetectorConfig(TL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index)
{
	if (index >= TL::AudioUnit::frequencyBandCount)
	{
		return TL::AudioUnit::Error::ERROR_INVALID_ARGUMENT;
	}

	TL::AudioUnit::deviceFunction = 101;
	Wire.beginTransmission(TL::AudioUnit::deviceAddress);
	Wire.write(TL::AudioUnit::deviceFunction);
	Wire.write(0);
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.historySize), sizeof(peakDetectorConfig.historySize));
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.threshold), sizeof(peakDetectorConfig.threshold));
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.influence), sizeof(peakDetectorConfig.influence));
	Wire.write(reinterpret_cast<uint8_t *>(&peakDetectorConfig.noiseGate), sizeof(peakDetectorConfig.noiseGate));

	return Wire.endTransmission(true) == 0 ? TL::AudioUnit::Error::OK : TL::AudioUnit::Error::ERROR_IIC_COMMUNICATION;
}
