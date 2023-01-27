/**
 * @file SparkleAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TL::SparkleAnimator}.
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
#include "led/animator/SparkleAnimator.h"

/**
 * @brief Create a new instance of {@link TL::SparkleAnimator}.
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
TL::SparkleAnimator::SparkleAnimator(const TL::SparkleAnimator::SpawnPosition spawnPosition, const uint8_t sparkCount, const CRGB color,
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
 * @brief Destroy the {@link TL::SparkleAnimator} instance.
 */
TL::SparkleAnimator::~SparkleAnimator()
{
}

/**
 * @brief Initialize the {@link TL::SparkleAnimator}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::SparkleAnimator::init(std::vector<CRGB> &pixels)
{
	this->pixelBuffer.assign(pixels.size(), CRGB::Black);
	this->pixelMask.assign(pixels.size(), false);
	std::fill(pixels.begin(), pixels.end(), CRGB::Black);
	this->colorAngle = 0.0f;
	this->audioSequence = 0;
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		TL::SparkleAnimator::Spark spark = this->sparks.at(i);
		spark.visible = false;
		spark.position = 0.0f;
		spark.speed = 0.0f;
		spark.friction = 0.0f;
		spark.color = CRGB::Black;
		spark.brightness = 0.0f;
		spark.fading = 0.0f;
		this->sparks.at(i) = spark;
	}
}

/**
 * @brief Render the sparkle animator.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::SparkleAnimator::render(std::vector<CRGB> &pixels)
{
	// Check if new sparks should be spawned depending on the data source
	if (this->getDataSource() == TL::LedAnimator::DataSource::DS_AUDIO_FREQUENCY_TRIGGER)
	{
		// Get the audio frequency analysis
		const TL::AudioUnit::AudioAnalysis audioAnalysis = this->getAudioAnalysis();
		if (audioAnalysis.frequencyBandTriggers.size() == AUDIO_UNIT_NUM_BANDS && audioAnalysis.seq != this->audioSequence)
		{
			// Check the sequency number
			this->audioSequence = audioAnalysis.seq;
			for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
			{
				// Spawn new sparks depending on the band mask and the tigger status
				if (this->frequencyBandMask & (0B10000000 >> i) && audioAnalysis.frequencyBandTriggers.at(i).trigger == TL::AudioUnit::Trigger::TRIGGER_RISING)
				{
					this->spawnSparks(pixels);
				}
			}
		}
	}
	else
	{
		this->spawnSparks(pixels);
	}

	// Run the sparks
	this->runSparks(pixels);

	// Clear the mask to store for which pixels a spark was rendere
	std::fill(this->pixelMask.begin(), this->pixelMask.end(), false);

	// Render the sparks and remember each pixel at which a spark is present
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		const TL::SparkleAnimator::Spark spark = this->sparks.at(i);
		if (spark.visible)
		{
			const CRGB pixel = this->pixelBuffer.at(spark.position);
			int16_t red = pixel.r;
			int16_t green = pixel.g;
			int16_t blue = pixel.b;

			float exposure = abs(spark.speed);
			exposure = exposure <= 1.0f ? exposure : 1.0f;

			red += spark.color.r * spark.brightness * exposure;
			green += spark.color.g * spark.brightness * exposure;
			blue += spark.color.b * spark.brightness * exposure;

			red = red <= 255 ? red : 255;
			green = green <= 255 ? green : 255;
			blue = blue <= 255 ? blue : 255;

			this->pixelBuffer.at(spark.position).setRGB(red, green, blue);
			this->pixelMask.at(spark.position) = true;
		}
	}

	// Slowly fade down the pixel buffer to achieve the tail effect
	// Only pixels where no spark is present are faded down
	for (size_t i = 0; i < this->pixelBuffer.size(); i++)
	{
		if (!this->pixelMask.at(i))
		{
			const CRGB pixel = this->pixelBuffer.at(i);
			int16_t red = pixel.r;
			int16_t green = pixel.g;
			int16_t blue = pixel.b;

			red -= red * (1.0f - this->sparkTail);
			green -= green * (1.0f - this->sparkTail);
			blue -= blue * (1.0f - this->sparkTail);

			red = red >= 0 ? red : 0;
			green = green >= 0 ? green : 0;
			blue = blue >= 0 ? blue : 0;

			this->pixelBuffer.at(i).setRGB(red, green, blue);
		}
	}

	pixels = this->pixelBuffer;
	if (this->reverse)
	{
		this->reversePixels(pixels);
	}
	this->applyBrightness(pixels);
}

/**
 * @brief Spawn new sparks from the set of currently invisible ones.
 */
void TL::SparkleAnimator::spawnSparks(std::vector<CRGB> &pixels)
{
	size_t spawnCounter = 0;
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		if (this->getDataSource() == TL::LedAnimator::DataSource::DS_AUDIO_FREQUENCY_TRIGGER)
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

		TL::SparkleAnimator::Spark spark = this->sparks.at(i);
		if (!spark.visible)
		{
			spark.visible = true;

			switch (this->spawnPosition)
			{
			case TL::SparkleAnimator::SpawnPosition::SPAWN_SIDE:
				spark.position = (this->offset / 255.0f) * (pixels.size() - 1.0f);
				spark.position += this->random(0, pixels.size() - 1.0f) * this->spawnVariance;
				spark.position = spark.position >= 0 ? spark.position : 0;
				spark.position = spark.position < pixels.size() ? spark.position : pixels.size() - 1;
				break;
			case TL::SparkleAnimator::SpawnPosition::SPAWN_CENTER:
				spark.position = pixels.size() / 2.0f;
				spark.position += this->random(-spark.position, spark.position) * this->spawnVariance;
				spark.position = spark.position >= 0 ? spark.position : 0;
				spark.position = spark.position < pixels.size() ? spark.position : pixels.size() - 1;
				break;
			case TL::SparkleAnimator::SpawnPosition::SPAWN_RANDOM:
				spark.position = this->random(0, pixels.size() - 1);
				break;
			}

			spark.speed = this->speed / 255.0f;
			spark.speed += (this->random(-255, 255) / 255.0f) * this->speedVariance;
			spark.speed = this->random(0, 255) < 127 ? -spark.speed : spark.speed;
			spark.speed = spark.speed < 0.05f && spark.speed > 0.0f ? 0.05f : spark.speed;
			spark.speed = spark.speed > -0.05f && spark.speed < 0.0f ? -0.05f : spark.speed;
			spark.speed = spark.speed > 1.0f ? 1.0f : spark.speed;
			spark.speed = spark.speed < -1.0f ? -1.0f : spark.speed;

			spark.friction = this->sparkFriction;
			spark.friction += (this->random(-255, 255) / 255.0f) * this->frictionVariance;
			spark.friction = spark.friction < 0.0f ? -spark.friction : spark.friction;
			spark.friction = spark.friction > 1.0f ? 1.0f : spark.friction;

			if (this->color.r == 0 && this->color.g == 0 && this->color.b == 0)
			{
				spark.color.setRGB(this->trapezoid(this->colorAngle) * 255.0f,
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
 */
void TL::SparkleAnimator::runSparks(std::vector<CRGB> &pixels)
{
	for (size_t i = 0; i < this->sparks.size(); i++)
	{
		TL::SparkleAnimator::Spark spark = this->sparks.at(i);
		if (spark.visible)
		{
			spark.position += spark.speed;
			spark.speed -= spark.speed * spark.friction;
			spark.brightness -= spark.brightness * spark.fading;

			if (spark.position < 0.0f || spark.position >= pixels.size())
			{
				if (this->bounceAtCorner)
				{
					spark.position = spark.position < 0.0f ? 0.0f : pixels.size() - 1;
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
