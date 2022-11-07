/**
 * @file StaticColorAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a static color
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef STATIC_COLOR_ANIMATOR_H
#define STATIC_COLOR_ANIMATOR_H

#include <math.h>
#include "led/animator/LedAnimator.h"

namespace TesLight
{
	class StaticColorAnimator : public LedAnimator
	{
	public:
		StaticColorAnimator();
		StaticColorAnimator(const CRGB color);
		~StaticColorAnimator();

		void init();
		void render();

		void setColor(const CRGB color);

	private:
		CRGB color;
	};
}

#endif