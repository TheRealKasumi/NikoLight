/**
 * @file SparkleAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@NL::SparkleAnimator}.
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
#include "led/animator/SparkleAnimator.h"

/**
 * @brief Create a new instance of {@link NL::SparkleAnimator}.
 * @param spawnPosition spawn position of the sparks
 * @param sparkCount number of sparks
 * @param color color of the particles, use pure black for a rainbow effect
 * @param sparkFriction friction of the sparks which is slowing them down in the range 0.0 to 1.0
 * @param sparkFading fading speed of the sparks in the range 0.0 to 1.0
 * @param sparktail tail length of the sparks in the range 0.0 to 1.0
 * @param birthRate probability with which a new spark is born each cycle/number of particles per audio peak
 * @param spawnVariance variance in the spawn position in the range 0.0 to 1.0
 * @param speedVariance variance in speed in the range 0.0 to 1.0
 * @param brightnessVariance variance in brightness in the range 0.0 to 1.0
 * @param frictionVariance variance in friction in the range 0.0 to 1.0
 * @param fadingVariance variance in fading in the range 0.0 to 1.0
 * @param bounceAtCorner when set to true, the particles will bounce at the end of the LED strip
 * @param frequencyBandMask bit mask to mask frequency bands in audio mode
 */
NL::SparkleAnimator::SparkleAnimator(const NL::SparkleAnimator::SpawnPosition spawnPosition, const uint8_t sparkCount, const NL::Pixel color,
									 const float sparkFriction, const float sparkFading, const float sparkTail, const float birthRate,
									 const float spawnVariance, const float speedVariance, const float brightnessVariance, const float frictionVariance,
									 const float fadingVariance, const bool bounceAtCorner, const uint8_t frequencyBandMask)
{
	this->spawnPosition = spawnPosition;
	this->sparks.resize(sparkCount);
	this->color = color;
	this->sparkFriction = sparkFriction;
	this->sparkFading = sparkFading;
	this->sparkTail = sparkTail;
	this->birthRate = birthRate;
	this->spawnVariance = spawnVariance;
	this->speedVariance = speedVariance;
	this->brightnessVariance = brightnessVariance;
	this->frictionVariance = frictionVariance;
	this->fadingVariance = fadingVariance;
	this->bounceAtCorner = bounceAtCorner;
	this->frequencyBandMask = frequencyBandMask;
	this->colorAngle = 0.0f;
	this->audioSequence = 0;

	this->limit((uint16_t)0, (uint16_t)255, this->offset);
	this->limit(0.0f, 1.0f, this->sparkFriction);
	this->limit(0.0f, 1.0f, this->sparkFading);
	this->limit(0.0f, 1.0f, this->sparkTail);
	this->limit(0.0f, 1.0f, this->birthRate);
	this->limit(0.0f, 1.0f, this->spawnVariance);
	this->limit(0.0f, 1.0f, this->speedVariance);
	this->limit(0.0f, 1.0f, this->brightnessVariance);
	this->limit(0.0f, 1.0f, this->frictionVariance);
	this->limit(0.0f, 1.0f, this->fadingVariance);
}

/**
 * @brief Destroy the {@link NL::SparkleAnimator} instance.
 */
NL::SparkleAnimator::~SparkleAnimator()
{
}

/**
 * @brief Initialize the {@link NL::SparkleAnimator}.
 * @param ledStrip LED strip with the pixel data
 */
void NL::SparkleAnimator::init(NL::LedStrip &ledStrip)
{
	this->pixelBuffer.assign(ledStrip.getLedCount(), NL::Pixel::ColorCode::Black);
	this->pixelMask.assign(ledStrip.getLedCount(), false);
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
	this->colorAngle = 0.0f;
	this->audioSequence = 0;
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		NL::SparkleAnimator::Spark spark = this->sparks.at(i);
		spark.visible = false;
		spark.position = 0.0f;
		spark.lastPosition = 0.0f;
		spark.speed = 0.0f;
		spark.friction = 0.0f;
		spark.color = NL::Pixel::ColorCode::Black;
		spark.brightness = 0.0f;
		spark.fading = 0.0f;
		this->sparks.at(i) = spark;
	}
}

/**
 * @brief Render the sparkle animator.
 * @param ledStrip LED strip with the pixel data
 */
void NL::SparkleAnimator::render(NL::LedStrip &ledStrip)
{
	// Check if new sparks should be spawned depending on the data source
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
				// Spawn new sparks depending on the band mask and the tigger status
				if (this->frequencyBandMask & (0B10000000 >> i) && audioAnalysis.frequencyBandTriggers.at(i).trigger == NL::AudioUnit::Trigger::TRIGGER_RISING)
				{
					this->spawnSparks(ledStrip);
				}
			}
		}
	}
	else
	{
		this->spawnSparks(ledStrip);
	}

	// Run the sparks
	this->runSparks(ledStrip);

	// Clear the mask to store for which pixels a spark was rendere
	std::fill(this->pixelMask.begin(), this->pixelMask.end(), false);

	// Render the sparks and remember each pixel at which a spark is present
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		const NL::SparkleAnimator::Spark spark = this->sparks.at(i);
		if (spark.visible)
		{
			float exposure = abs(spark.speed);
			exposure = exposure <= 1.0f ? exposure : 1.0f;

			const float dist = spark.position - spark.lastPosition;
			const bool isPositive = dist >= 0;
			const float absDist = abs(dist);
			for (size_t j = 0; j < absDist; j++)
			{
				if (isPositive && spark.position - j < 0)
				{
					break;
				}
				else if (!isPositive && spark.position + j > ledStrip.getLedCount())
				{
					break;
				}

				const NL::Pixel pixel = this->pixelBuffer.at(isPositive ? spark.position - j : spark.position + j);
				int16_t red = pixel.red;
				int16_t green = pixel.green;
				int16_t blue = pixel.blue;

				red += spark.color.red * spark.brightness * exposure;
				green += spark.color.green * spark.brightness * exposure;
				blue += spark.color.blue * spark.brightness * exposure;

				red = red <= 255 ? red : 255;
				green = green <= 255 ? green : 255;
				blue = blue <= 255 ? blue : 255;

				this->pixelBuffer.at(isPositive ? spark.position - j : spark.position + j).setColor(red, green, blue);
			}
			this->pixelMask.at(spark.position) = true;
		}
	}

	// Slowly fade down the pixel buffer to achieve the tail effect
	// Only pixels where no spark is present are faded down
	for (size_t i = 0; i < this->pixelBuffer.size(); i++)
	{
		if (!this->pixelMask.at(i))
		{
			const NL::Pixel pixel = this->pixelBuffer.at(i);
			int16_t red = pixel.red;
			int16_t green = pixel.green;
			int16_t blue = pixel.blue;

			red -= red * (1.0f - this->sparkTail);
			green -= green * (1.0f - this->sparkTail);
			blue -= blue * (1.0f - this->sparkTail);

			red = red >= 0 ? red : 0;
			green = green >= 0 ? green : 0;
			blue = blue >= 0 ? blue : 0;

			this->pixelBuffer.at(i).setColor(red, green, blue);
		}
	}

	// Copy the internal pixel buffer into the LED strip
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(this->pixelBuffer.at(i), i);
	}

	if (this->reverse)
	{
		this->reversePixels(ledStrip);
	}
	this->applyBrightness(ledStrip);
}

/**
 * @brief Spawn new sparks from the set of currently invisible ones.
 * @param ledStrip reference to the LED strip
 */
void NL::SparkleAnimator::spawnSparks(NL::LedStrip &ledStrip)
{
	size_t spawnCounter = 0;
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		if (this->getDataSource() == NL::LedAnimator::DataSource::DS_AUDIO_FREQUENCY_TRIGGER)
		{
			if (spawnCounter >= this->birthRate * this->sparks.size())
			{
				break;
			}
		}
		else
		{
			if (this->random(0, 1000) < (1000.0f - this->birthRate * 1000.0f))
			{
				continue;
			}
		}

		NL::SparkleAnimator::Spark spark = this->sparks.at(i);
		if (!spark.visible)
		{
			spark.visible = true;

			switch (this->spawnPosition)
			{
			case NL::SparkleAnimator::SpawnPosition::SPAWN_SIDE:
				spark.position = (this->offset / 255.0f) * (ledStrip.getLedCount() - 1.0f);
				spark.position += this->random(0, ledStrip.getLedCount() - 1.0f) * this->spawnVariance;
				spark.position = spark.position >= 0 ? spark.position : 0;
				spark.position = spark.position < ledStrip.getLedCount() ? spark.position : ledStrip.getLedCount() - 1;
				spark.lastPosition = spark.position;
				break;
			case NL::SparkleAnimator::SpawnPosition::SPAWN_CENTER:
				spark.position = ledStrip.getLedCount() / 2.0f;
				spark.position += this->random(-spark.position, spark.position) * this->spawnVariance;
				spark.position = spark.position >= 0 ? spark.position : 0;
				spark.position = spark.position < ledStrip.getLedCount() ? spark.position : ledStrip.getLedCount() - 1;
				spark.lastPosition = spark.position;
				break;
			case NL::SparkleAnimator::SpawnPosition::SPAWN_RANDOM:
				spark.position = this->random(0, ledStrip.getLedCount() - 1);
				spark.lastPosition = spark.position;
				break;
			}

			spark.speed = this->speed / 64.0f;
			spark.speed += (this->random(-255, 255) / 64.0f) * this->speedVariance;
			spark.speed = this->random(0, 255) < 127 ? -spark.speed : spark.speed;
			spark.speed = spark.speed < 0.05f && spark.speed > 0.0f ? 0.05f : spark.speed;
			spark.speed = spark.speed > -0.05f && spark.speed < 0.0f ? -0.05f : spark.speed;
			spark.speed = spark.speed > 3.0f ? 3.0f : spark.speed;
			spark.speed = spark.speed < -3.0f ? -3.0f : spark.speed;

			spark.friction = this->sparkFriction;
			spark.friction += (this->random(-255, 255) / 255.0f) * this->frictionVariance;
			spark.friction = spark.friction < 0.0f ? -spark.friction : spark.friction;
			spark.friction = spark.friction > 1.0f ? 1.0f : spark.friction;

			if (this->color.red == 0 && this->color.green == 0 && this->color.blue == 0)
			{
				spark.color.setColor(this->trapezoid(this->colorAngle) * 255.0f,
									 this->trapezoid(this->colorAngle + 120.0f) * 255.0f,
									 this->trapezoid(this->colorAngle + 240.0f) * 255.0f);
				this->colorAngle += 7.5f;
				if (this->colorAngle > 360.0f)
				{
					this->colorAngle -= 360.0f;
				}
			}
			else
			{
				spark.color = this->color;
			}

			spark.brightness = 1.0f - (this->random(0, 255) / 255.0f) * this->brightnessVariance;

			spark.fading = this->sparkFading;
			spark.fading += (this->random(-255, 255) / 255.0f) * this->fadingVariance;
			spark.fading = spark.fading < 0.0f ? -spark.fading : spark.fading;
			spark.fading = spark.fading > 1.0f ? 1.0f : spark.fading;

			this->sparks.at(i) = spark;
			spawnCounter++;
		}
	}
}

/**
 * @brief Run the sparks :) . I know, thats a great explanation.
 * @param ledStrip reference to the LED strip
 */
void NL::SparkleAnimator::runSparks(NL::LedStrip &ledStrip)
{
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		NL::SparkleAnimator::Spark spark = this->sparks.at(i);
		if (spark.visible)
		{
			spark.lastPosition = spark.position;
			spark.position += spark.speed;
			spark.speed -= spark.speed * spark.friction;
			spark.brightness -= spark.brightness * spark.fading;

			if (spark.position < 0.0f || spark.position >= ledStrip.getLedCount())
			{
				if (this->bounceAtCorner)
				{
					spark.position = spark.position < 0.0f ? 0.0f : ledStrip.getLedCount() - 1;
					spark.lastPosition = spark.position;
					spark.speed = -spark.speed;
				}
				else
				{
					spark.visible = false;
				}
			}

			if (spark.brightness * this->animationBrightness * this->ambientBrightness <= 1.0f / 255.0f || abs(spark.speed) < 1.0 / 60.0f)
			{
				spark.visible = false;
			}

			this->sparks.at(i) = spark;
		}
	}
}
