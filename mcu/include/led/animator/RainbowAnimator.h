/**
 * @file RainbowAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a rainbow.
 * @version 0.0.1
 * @date 2022-06-28
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
	enum RainbowMode
	{
		RAINBOW_LINEAR,
		RAINBOW_CENTER
	};

	class RainbowAnimator : public LedAnimator
	{
	public:
		RainbowAnimator();
		~RainbowAnimator();

		void init();
		void render();

		void setRainbowMode(const TesLight::RainbowMode rainbowMode);

	private:
		float angle;
		TesLight::RainbowMode rainbowMode;
	};
}

#endif