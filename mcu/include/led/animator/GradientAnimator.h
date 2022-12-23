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

#include <vector>
#include "led/animator/LedAnimator.h"

namespace TL
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
		GradientAnimator(const TL::GradientAnimator::GradientMode gradientMode, const CRGB color1, const CRGB color2);
		~GradientAnimator();

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

		void setGradientMode(const TL::GradientAnimator::GradientMode gradientMode);
		void setColor(const CRGB color1, const CRGB color2);

	private:
		TL::GradientAnimator::GradientMode gradientMode;
		CRGB color[2];
	};
}

#endif