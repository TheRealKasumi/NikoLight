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

namespace TL
{
	class SparkleAnimator : public LedAnimator
	{
	public:
		enum SpawnPosition
		{
			SPAWN_LEFT = 0,
			SPAWN_RIGHT = 1,
			SPAWN_CENTER = 2,
			SPAWN_RANDOM = 3
		};

		struct Spark
		{
			bool visible;
			float position;
			float speed;
			float friction;
			CRGB color;
			float brightness;
			float fading;
		};

		SparkleAnimator(const TL::SparkleAnimator::SpawnPosition spawnPosition, const uint8_t sparkCount, const CRGB color,
						const float sparkFriction, const float sparkFading, const float sparkTail, const float birthRate,
						const float spawnVariance, const float speedVariance, const float brightnessVariance, const float frictionVariance,
						const float fadingVariance, const bool bounceAtCorner);
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

		float colorAngle;

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