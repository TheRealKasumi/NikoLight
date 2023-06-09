/**
 * @file RainbowAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a rainbow.
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
#ifndef RAINBOW_ANIMATOR_H
#define RAINBOW_ANIMATOR_H

#include <vector>
#include "led/animator/LedAnimator.h"

namespace NL
{
	class RainbowAnimator : public LedAnimator
	{
	public:
		enum class RainbowMode
		{
			RAINBOW_SOLID = 0,
			RAINBOW_LINEAR = 1,
			RAINBOW_CENTER = 2
		};

		RainbowAnimator(const NL::RainbowAnimator::RainbowMode rainbowMode);
		~RainbowAnimator();

		void init(NL::LedStrip &ledStrip);
		void render(NL::LedStrip &ledStrip);

	private:
		float angle;
		NL::RainbowAnimator::RainbowMode rainbowMode;
	};
}

#endif