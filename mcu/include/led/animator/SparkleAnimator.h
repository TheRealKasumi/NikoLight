/**
 * @file SparkleAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a particle/spark animation.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
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
#ifndef SPARKLE_ANIMATOR_H
#define SPARKLE_ANIMATOR_H

#include <vector>
#include "led/animator/LedAnimator.h"
#include "configuration/SystemConfiguration.h"

namespace TL
{
	class SparkleAnimator : public LedAnimator
	{
	public:
		enum class SpawnPosition : uint8_t
		{
			SPAWN_SIDE = 0,	  // Spawn new sparks at the side
			SPAWN_CENTER = 1, // Spawm new sparks at the middle
			SPAWN_RANDOM = 2  // Spawn new sparks at a random position
		};

		struct Spark
		{
			bool visible;		// Visibility of a spark
			float position;		// Position of a spark
			float lastPosition; // Previous position of a spark
			float speed;		// Speed of a spark
			float friction;		// Friction of a spark
			CRGB color;			// Color of a spark
			float brightness;	// Brightness of a spark
			float fading;		// Fafing speed of a spark
		};

		SparkleAnimator(const TL::SparkleAnimator::SpawnPosition spawnPosition, const uint8_t sparkCount, const CRGB color,
						const float sparkFriction, const float sparkFading, const float sparkTail, const float birthRate,
						const float spawnVariance, const float speedVariance, const float brightnessVariance, const float frictionVariance,
						const float fadingVariance, const bool bounceAtCorner, const uint8_t frequencyBandMask);
		~SparkleAnimator();

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

	private:
		std::vector<CRGB> pixelBuffer;
		std::vector<bool> pixelMask;
		std::vector<Spark> sparks;

		TL::SparkleAnimator::SpawnPosition spawnPosition;
		CRGB color;
		float sparkFriction;
		float sparkFading;
		float sparkTail;
		float birthRate;
		float spawnVariance;
		float speedVariance;
		float brightnessVariance;
		float frictionVariance;
		float fadingVariance;
		bool bounceAtCorner;
		uint8_t frequencyBandMask;

		float colorAngle;
		uint8_t audioSequence;

		void spawnSparks(std::vector<CRGB> &pixels);
		void runSparks(std::vector<CRGB> &pixels);

		template <typename T>
		void limit(const T min, const T max, T &value)
		{
			if (value < min)
			{
				value = min;
			}
			else if (value > max)
			{
				value = max;
			}
		}
	};
}

#endif