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
		StaticColorAnimator(const uint8_t red, const uint8_t green, const uint8_t blue);
		~StaticColorAnimator();

		void init();
		void render();

		void setColor(const uint8_t red, const uint8_t green, const uint8_t blue);

	private:
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
}

#endif