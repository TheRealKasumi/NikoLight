/**
 * @file AudioUnit.h
 * @author TheRealKasumi
 * @brief Contains a class for connecting to the TesLight Audio Unit.
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
#ifndef TL_AUDIO_UNIT_H
#define TL_AUDIO_UNIT_H

#include <stdint.h>
#include <vector>
#include <tuple>
#include <Wire.h>

namespace TL
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

		enum Trigger : uint8_t
		{
			TRIGGER_LOW = 0,	// No peak detected
			TRIGGER_RISING = 1, // Peak got just detected in the current frame
			TRIGGER_HIGH = 2,	// We are still at a peak level
			TRIGGER_FALLING = 3 // Peak ended
		};

		struct PeakResult
		{
			TL::AudioUnit::Trigger trigger; // Trigger to check if a peak is detected
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
			std::vector<TL::AudioUnit::PeakResult> frequencyBandTriggers; // Trigger for each frequency band
		};

		static TL::AudioUnit::Error begin(const uint8_t deviceAddress);
		static void end();
		static bool isInitialized();

		static uint8_t getFrequencyBandCount();

		static TL::AudioUnit::Error getAudioAnalysis(TL::AudioUnit::AudioAnalysis &audioAnalysis);

		static TL::AudioUnit::Error getAudioUnitConfig(TL::AudioUnit::AudioUnitConfig &analyzerConfig);
		static TL::AudioUnit::Error setAudioUnitConfig(TL::AudioUnit::AudioUnitConfig &analyzerConfig);

		static TL::AudioUnit::Error getPeakDetectorConfig(TL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index);
		static TL::AudioUnit::Error setPeakDetectorConfig(TL::AudioUnit::PeakDetectorConfig &peakDetectorConfig, const uint8_t index);

	private:
		AudioUnit();

		static bool initialized;
		static uint8_t deviceAddress;
		static uint8_t deviceFunction;
		static uint8_t frequencyBandCount;
	};
}

#endif
