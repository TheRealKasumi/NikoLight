/**
 * @file ColorBarAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render moving color bars with a variable speed and color.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef COLOR_BAR_ANIMATOR_H
#define COLOR_BAR_ANIMATOR_H

#include "led/animator/LedAnimator.h"

namespace TesLight
{
	class ColorBarAnimator : public LedAnimator
	{
	public:
		enum ColorBarMode
		{
			COLOR_BAR_LINEAR_HARD,
			COLOR_BAR_LINEAR_SMOOTH,
			COLOR_BAR_CENTER_HARD,
			COLOR_BAR_CENTER_SMOOTH
		};

		ColorBarAnimator();
		ColorBarAnimator(const TesLight::ColorBarAnimator::ColorBarMode colorBarMode, const CRGB color1, const CRGB color2);
		~ColorBarAnimator();

		void init();
		void render();

		void setColorBarMode(const TesLight::ColorBarAnimator::ColorBarMode colorBarMode);
		void setColor(const CRGB color1, const CRGB color2);

	private:
		float angle;
		TesLight::ColorBarAnimator::ColorBarMode colorBarMode;
		CRGB color[2];
	};
}

#endif