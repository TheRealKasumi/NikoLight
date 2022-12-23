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

#include <vector>
#include "led/animator/LedAnimator.h"

namespace TL
{
	class StaticColorAnimator : public LedAnimator
	{
	public:
		StaticColorAnimator();
		StaticColorAnimator(const CRGB color);
		~StaticColorAnimator();

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

		void setColor(const CRGB color);

	private:
		CRGB color;
	};
}

#endif