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

#include <vector>
#include "led/animator/LedAnimator.h"

namespace TL
{
	class RainbowAnimator : public LedAnimator
	{
	public:
		enum RainbowMode
		{
			RAINBOW_SOLID,
			RAINBOW_LINEAR,
			RAINBOW_CENTER
		};

		RainbowAnimator();
		RainbowAnimator(const TL::RainbowAnimator::RainbowMode rainbowMode);
		~RainbowAnimator();

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

		void setRainbowMode(const TL::RainbowAnimator::RainbowMode rainbowMode);

	private:
		float angle;
		TL::RainbowAnimator::RainbowMode rainbowMode;
	};
}

#endif