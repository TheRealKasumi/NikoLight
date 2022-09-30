/**
 * @file RainbowAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a rainbow.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAINBOW_ANIMATOR_H
#define RAINBOW_ANIMATOR_H

#include <math.h>
#include "led/animator/LedAnimator.h"

namespace TesLight
{
	class RainbowAnimator : public LedAnimator
	{
	public:
		enum RainbowMode
		{
			RAINBOW_SOLID,
			RAINBOW_LINEAR,
			RAINBOW_CENTER,
			RAINBOW_SLIDE
		};

		RainbowAnimator();
		RainbowAnimator(const TesLight::RainbowAnimator::RainbowMode rainbowMode);
		~RainbowAnimator();

		void init();
		void render();

		void setRainbowMode(const TesLight::RainbowAnimator::RainbowMode rainbowMode);

	private:
		float angle;
		TesLight::RainbowAnimator::RainbowMode rainbowMode;
	};
}

#endif