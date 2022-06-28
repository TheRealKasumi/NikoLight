/**
 * @file GradientAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a gradient.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef GRADIENT_ANIMATOR_H
#define GRADIENT_ANIMATOR_H

#include <math.h>
#include "led/driver/LedAnimator.h"

namespace TesLight
{
	class GradientAnimator : public LedAnimator
	{
	public:
		GradientAnimator();
		~GradientAnimator();

		void init();
		void render();

		void setColor(const uint8_t red1, const uint8_t green1, const uint8_t blue1, const uint8_t red2, const uint8_t green2, const uint8_t blue2);

	private:
		uint8_t red[2];
		uint8_t green[2];
		uint8_t blue[2];
	};
}

#endif