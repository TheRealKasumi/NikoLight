/**
 * @file SparkleAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a particle/spark animation.
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
#ifndef SPARKLE_ANIMATOR_H
#define SPARKLE_ANIMATOR_H

#include <vector>
#include "led/animator/LedAnimator.h"
#include "configuration/SystemConfiguration.h"

namespace NL
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
			NL::Pixel color;	// Color of a spark
			float brightness;	// Brightness of a spark
			float fading;		// Fafing speed of a spark
		};

		SparkleAnimator(const NL::SparkleAnimator::SpawnPosition spawnPosition, const uint8_t sparkCount, const NL::Pixel color,
						const float sparkFriction, const float sparkFading, const float sparkTail, const float birthRate,
						const float spawnVariance, const float speedVariance, const float brightnessVariance, const float frictionVariance,
						const float fadingVariance, const bool bounceAtCorner, const uint8_t frequencyBandMask);
		~SparkleAnimator();

		void init(NL::LedStrip &ledStrip);
		void render(NL::LedStrip &ledStrip);

	private:
		std::vector<NL::Pixel> pixelBuffer;
		std::vector<bool> pixelMask;
		std::vector<Spark> sparks;

		NL::SparkleAnimator::SpawnPosition spawnPosition;
		NL::Pixel color;
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

		void spawnSparks(NL::LedStrip &ledStrip);
		void runSparks(NL::LedStrip &ledStrip);

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