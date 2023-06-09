/**
 * @file GradientAnimatorMotion.h
 * @author TheRealKasumi
 * @brief Contains a class to render a gradient based on the cars motion.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef GRADIENT_ANIMATOR_MOTION_H
#define GRADIENT_ANIMATOR_MOTION_H

#include <vector>
#include "led/animator/LedAnimator.h"
#include "sensor/MotionSensor.h"

namespace NL
{
	class GradientAnimatorMotion : public LedAnimator
	{
	public:
		enum class GradientMode : uint8_t
		{
			GRADIENT_LINEAR = 0,
			GRADIENT_CENTER = 1
		};

		GradientAnimatorMotion(const NL::GradientAnimatorMotion::GradientMode gradientMode, const NL::Pixel color1, const NL::Pixel color2);
		~GradientAnimatorMotion();

		void init(NL::LedStrip &ledStrip);
		void render(NL::LedStrip &ledStrip);

	private:
		NL::GradientAnimatorMotion::GradientMode gradientMode;
		NL::Pixel color[2];

		float getMotionOffset();
	};
}

#endif