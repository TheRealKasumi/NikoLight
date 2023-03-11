/**
 * @file EqualizerAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TL::EqualizerAnimator}.
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
#include "led/animator/EqualizerAnimator.h"

/**
 * @brief Create a new instance of {@link TL::EqualizerAnimator}.
 * @param color1 start color (use black for rainbow)
 * @param color2 end color (use black for rainbow)
 * @param rainbowSpeed speed of the color change in case of rainbow color
 * @param autoGain strength of the automatic gain
 * @param frequencyBandMask bit mask to mask frequency bands in audio mode
 */
TL::EqualizerAnimator::EqualizerAnimator(const TL::Pixel color1, const TL::Pixel color2, const uint8_t rainbowSpeed, const float autoGain, const uint8_t frequencyBandMask)
{
	this->color[0] = color1;
	this->color[1] = color2;
	this->rainbowSpeed = rainbowSpeed;
	this->autoGain = autoGain;
	this->frequencyBandMask = frequencyBandMask;

	this->audioSequence = 0;
	this->currentVolume = 0.0f;
	this->peakVolume = 1000.0f;
	this->colorAngle = 0.0f;
	this->rainbowMode[0] = this->color[0].red == 0 && this->color[0].green == 0 && this->color[0].blue == 0;
	this->rainbowMode[1] = this->color[1].red == 0 && this->color[1].green == 0 && this->color[1].blue == 0;

	if (this->autoGain < 0.0f)
	{
		this->autoGain = 0.0f;
	}
	else if (this->autoGain > 1.0f)
	{
		this->autoGain = 1.0f;
	}
}

/**
 * @brief Destroy the {@link TL::EqualizerAnimator} instance.
 */
TL::EqualizerAnimator::~EqualizerAnimator()
{
}

/**
 * @brief Initialize the {@link TL::EqualizerAnimator}.
 * @param pixels LED strip with the pixel data
 */
void TL::EqualizerAnimator::init(TL::LedStrip &ledStrip)
{
	this->audioSequence = 0;
	this->audioSequence = 0;
	this->currentVolume = 0.0f;
	this->peakVolume = 1000.0f;
	this->colorAngle = 0.0f;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(TL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the equalizer animation.
 * @param pixels LED strip with the pixel data
 */
void TL::EqualizerAnimator::render(TL::LedStrip &ledStrip)
{
	// Check the data source
	if (this->getDataSource() == TL::LedAnimator::DataSource::DS_AUDIO_FREQUENCY_VALUE)
	{
		// Check the sequence number
		const TL::AudioUnit::AudioAnalysis audioAnalysis = this->getAudioAnalysis();
		if (audioAnalysis.frequencyBandTriggers.size() == AUDIO_UNIT_NUM_BANDS && audioAnalysis.seq != this->audioSequence)
		{
			this->audioSequence = audioAnalysis.seq;
			for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
			{
				// Determine the peak value from all frequency bands
				const uint16_t peak = audioAnalysis.frequencyBandValues.at(i);
				if (this->frequencyBandMask & (0B10000000 >> i))
				{
					// Update the volume when the peak is higher
					if (peak > this->currentVolume)
					{
						this->currentVolume = peak;
					}

					// Update the maximum volume of the bar when the limit is exceeded
					if (peak >= this->peakVolume)
					{
						this->peakVolume = peak;
					}
				}
			}
		}
	}
	else
	{
		// Use the random mode
		const uint16_t peak = pow(this->random(0, 10), 3);

		// Update the volume when the peak is higher
		if (peak > this->currentVolume)
		{
			this->currentVolume = peak;
		}

		// Update the maximum volume of the bar when the limit is exceeded
		if (peak >= this->peakVolume)
		{
			this->peakVolume = peak;
		}
	}

	// Determine the color in case of the rainbow mode
	if (this->rainbowMode[0])
	{
		this->color[0].red = this->trapezoid(this->colorAngle + 0) * 255.0f;
		this->color[0].green = this->trapezoid(this->colorAngle + 120) * 255.0f;
		this->color[0].blue = this->trapezoid(this->colorAngle + 240) * 255.0f;
	}
	if (this->rainbowMode[1])
	{
		const uint16_t offset = 60;
		this->color[1].red = this->trapezoid(this->colorAngle + offset + 0) * 255.0f;
		this->color[1].green = this->trapezoid(this->colorAngle + offset + 120) * 255.0f;
		this->color[1].blue = this->trapezoid(this->colorAngle + offset + 240) * 255.0f;
	}

	// Render the volume bar
	const float middle = (this->offset / 255.0f) * ledStrip.getLedCount();
	const float barWidth = this->currentVolume / this->peakVolume;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		const float distance = i < middle ? (1.0f - i / middle) : ((i - middle) / (ledStrip.getLedCount() - middle));
		if (distance < barWidth)
		{
			ledStrip.setPixel(
				TL::Pixel(
					this->color[0].red * (1.0f - distance) + this->color[1].red * distance,
					this->color[0].green * (1.0f - distance) + this->color[1].green * distance,
					this->color[0].blue * (1.0f - distance) + this->color[1].blue * distance),
				i);
		}
		else if (distance < barWidth + 0.05f)
		{
			ledStrip.setPixel(
				TL::Pixel(
					(this->color[0].red * (1.0f - distance) + this->color[1].red * distance) * ((barWidth + 0.05f) - distance) * 20.0f,
					(this->color[0].green * (1.0f - distance) + this->color[1].green * distance) * ((barWidth + 0.05f) - distance) * 20.0f,
					(this->color[0].blue * (1.0f - distance) + this->color[1].blue * distance) * ((barWidth + 0.05f) - distance) * 20.0f),
				i);
		}
		else
		{
			ledStrip.setPixel(TL::Pixel::ColorCode::Black, i);
		}
	}

	// Reduce the volume with each iteration
	this->currentVolume -= this->peakVolume * ((this->speed / 255.0f) / 15.0f);
	if (this->currentVolume < 0.0f)
	{
		this->currentVolume = 0.0f;
	}

	// Handle the auto gain to slowly adjust to the volume
	this->peakVolume += (this->currentVolume - this->peakVolume) * (this->autoGain / 150.0f);
	if (this->peakVolume < 1000.0f)
	{
		this->peakVolume = 1000.0f;
	}

	// Change the color angle in case of a rainbow color
	this->colorAngle += this->rainbowSpeed / 50.0f;
	if (this->colorAngle >= 360.0f)
	{
		this->colorAngle -= 360.0f;
	}
	else if (this->colorAngle < 0.0f)
	{
		this->colorAngle += 360.0f;
	}

	// Eventually reverse the LEDs
	if (this->reverse)
	{
		this->reversePixels(ledStrip);
	}

	// Apply the brightness settings
	this->applyBrightness(ledStrip);
}
