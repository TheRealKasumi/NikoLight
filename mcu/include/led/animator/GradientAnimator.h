/**
 * @file GradientAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a gradient.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef GRADIENT_ANIMATOR_H
#define GRADIENT_ANIMATOR_H

#include <math.h>
#include "led/animator/LedAnimator.h"

namespace TesLight
{
	class GradientAnimator : public LedAnimator
	{
	public:
		enum GradientMode
		{
			GRADIENT_LINEAR,
			GRADIENT_CENTER
		};

		GradientAnimator();
		GradientAnimator(const TesLight::GradientAnimator::GradientMode gradientMode, const CRGB color1, const CRGB color2);
		~GradientAnimator();

		void init();
		void render();

		void setGradientMode(const TesLight::GradientAnimator::GradientMode gradientMode);
		void setColor(const CRGB color1, const CRGB color2);

	private:
		TesLight::GradientAnimator::GradientMode gradientMode;
		CRGB color[2];
	};
}

#endif