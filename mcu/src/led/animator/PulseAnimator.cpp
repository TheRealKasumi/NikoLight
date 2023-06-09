/**
 * @file PulseAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@NL::PulseAnimator}.
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
#include "led/animator/PulseAnimator.h"

/**
 * @brief Create a new instance of {@link NL::PulseAnimator}.
 * @param pulseMode mode for the pulse, linear or non-linear
 * @param color color of the pulses, rainbow when set to black
 * @param pulseFading speed with which the pulse will fade
 * @param frequencyBandMask bit mask to mask frequency bands in audio mode
 */
NL::PulseAnimator::PulseAnimator(const PulseMode pulseMode, const NL::Pixel color, const float pulseFading, const uint8_t frequencyBandMask)
{
	this->pulseMode = pulseMode;
	this->color = color;
	this->pulseFading = pulseFading;
	this->frequencyBandMask = frequencyBandMask;

	this->mode = 0;
	this->pulseBrightness = 0.0f;
	this->colorAngle = 0.0f;
	this->audioSequence = 0;

	if (this->pulseFading < 0.0f)
	{
		this->pulseFading = 0.0f;
	}
	else if (this->pulseFading > 1.0f)
	{
		this->pulseFading = 1.0f;
	}
}

/**
 * @brief Destroy the {@link NL::PulseAnimator} instance.
 */
NL::PulseAnimator::~PulseAnimator()
{
}

/**
 * @brief Initialize the {@link NL::PulseAnimator}.
 * @param ledStrip LED strip with the pixel data
 */
void NL::PulseAnimator::init(NL::LedStrip &ledStrip)
{
	this->mode = 0;
	this->pulseBrightness = 0.0f;
	this->colorAngle = 0.0f;
	this->audioSequence = 0;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the pulse animator.
 * @param ledStrip LED strip with the pixel data
 */
void NL::PulseAnimator::render(NL::LedStrip &ledStrip)
{
	// Check if a new pulse should be triggered depending on the data source
	if (this->getDataSource() == NL::LedAnimator::DataSource::DS_AUDIO_FREQUENCY_TRIGGER)
	{
		// Get the audio frequency analysis
		const NL::AudioUnit::AudioAnalysis audioAnalysis = this->getAudioAnalysis();
		if (audioAnalysis.frequencyBandTriggers.size() == AUDIO_UNIT_NUM_BANDS && audioAnalysis.seq != this->audioSequence)
		{
			// Check the sequency number
			this->audioSequence = audioAnalysis.seq;
			for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
			{
				// Trigger a new pulse depending on the band mask and the tigger status
				if (this->frequencyBandMask & (0B10000000 >> i) && audioAnalysis.frequencyBandTriggers.at(i).trigger == NL::AudioUnit::Trigger::TRIGGER_RISING)
				{
					this->mode = 2;
					this->pulseBrightness = 1.0f;
					this->colorAngle += this->reverse ? this->speed : -this->speed;
				}
			}
		}
	}
	else
	{
		// Start a new soft pulse when the previous one is faded
		if (this->mode == 0)
		{
			this->mode = 1;
			this->pulseBrightness = 0.003f;
			this->colorAngle += this->reverse ? this->speed : -this->speed;
		}
		else if (this->mode == 1)
		{
			this->pulseBrightness += this->pulseMode == NL::PulseAnimator::PulseMode::LINEAR ? this->pulseFading / 12.0f : this->pulseBrightness * this->pulseFading;
			if (this->pulseBrightness >= 1.0f)
			{
				this->mode = 2;
				this->pulseBrightness = 1.0f;
			}
		}
	}

	// Limit the color angle
	if (this->colorAngle >= 360.0f)
	{
		this->colorAngle -= 360.0f;
	}
	else if (this->colorAngle < 0.0f)
	{
		this->colorAngle += 360.0f;
	}

	// Render the LEDs
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		if (this->color.red == 0 && this->color.green == 0 && this->color.blue == 0)
		{
			// Rainbow mode
			ledStrip.setPixel(NL::Pixel(
								  this->trapezoid(this->colorAngle + 0.0f) * 255.0f * this->pulseBrightness,
								  this->trapezoid(this->colorAngle + 120.0f) * 255.0f * this->pulseBrightness,
								  this->trapezoid(this->colorAngle + 240.0f) * 255.0f * this->pulseBrightness),
							  i);
		}
		else
		{
			// Static color mode
			ledStrip.setPixel(NL::Pixel(this->color.red * this->pulseBrightness, this->color.green * this->pulseBrightness, this->color.blue * this->pulseBrightness), i);
		}
	}

	// Reduce the brightness of the pulse
	if (this->mode == 2)
	{
		this->pulseBrightness -= this->pulseMode == NL::PulseAnimator::PulseMode::LINEAR ? this->pulseFading / 12.0f : this->pulseBrightness * this->pulseFading;
		if (this->pulseBrightness < 0.003f)
		{
			this->pulseBrightness = 0.0f;
			this->mode = 0;
		}
	}

	// Apply the brightness adjustment
	this->applyBrightness(ledStrip);
}
