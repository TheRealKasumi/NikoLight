/**
 * @file AudioUnit.h
 * @author TheRealKasumi
 * @brief Contains a class for connecting to the NikoLight Audio Unit.
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
#ifndef NL_AUDIO_UNIT_H
#define NL_AUDIO_UNIT_H

#include <stdint.h>
#include <vector>
#include <tuple>
#include <Wire.h>

namespace NL
{
	class AudioUnit
	{
	public:
		enum class Error
		{
			OK,						 // No error
			ERROR_IIC_COMMUNICATION, // Failed to communicate with the device
			ERROR_INVALID_ARGUMENT	 // Argument is invalid
		};

		struct AudioUnitConfig
		{
			uint16_t noiseThreshold;									   // Threshold to filter out static noise
			std::vector<std::pair<uint16_t, uint16_t>> frequencyBandIndex; // Frequency band start and end bin indices
		};

		struct PeakDetectorConfig
		{
			uint16_t historySize; // Number of elements stored in the history
			double threshold;	  // Threshold to detect a peak
			double influence;	  // Influence of a triggering value on the mean
			double noiseGate;	  // Dont trigger when the value is below the noise gate
		};

		enum class Trigger : uint8_t
		{
			TRIGGER_LOW = 0,	// No peak detected
			TRIGGER_RISING = 1, // Peak got just detected in the current frame
			TRIGGER_HIGH = 2,	// We are still at a peak level
			TRIGGER_FALLING = 3 // Peak ended
		};

		struct PeakResult
		{
			NL::AudioUnit::Trigger trigger; // Trigger to check if a peak is detected
			double value;					// Intensity of the frequency band
			double mean;					// Mean value of the frequency band
			double standardDeviation;		// Standard eviation of the frequency band
			double triggerThreshold;		// Threshold where the trigger is set to active
		};

		struct AudioAnalysis
		{
			uint8_t seq;												  // Sequence number
			uint16_t volumePeak;										  // Maximum volume detected since the last cycle
			std::vector<uint16_t> frequencyBandValues;					  // Intensity values for each frequency band
			std::vector<NL::AudioUnit::PeakResult> frequencyBandTriggers; // Trigger for each frequency band
		};

		static NL::AudioUnit::Error begin(const uint8_t deviceAddress);
		static void end();
		static bool isInitialized();

		static uint8_t getFrequencyBandCount();

		static NL::AudioUnit::Error getAudioAnalysis(NL::AudioUnit::AudioAnalysis &audioAnalysis);

		static NL::AudioUnit::Error getAudioUnitConfig(NL::AudioUnit::AudioUnitConfig &analyzerConfig);
		static NL::AudioUnit::Error setAudioUnitConfig(NL::AudioUnit::AudioUnitConfig &analyzerConfig);

		static NL::AudioUnit::Error getPeakDetectorConfig(NL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index);
		static NL::AudioUnit::Error setPeakDetectorConfig(NL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index);

	private:
		AudioUnit();

		static bool initialized;
		static uint8_t deviceAddress;
		static uint8_t deviceFunction;
		static uint8_t frequencyBandCount;
	};
}

#endif
