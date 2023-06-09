/**
 * @file ColorBarAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render moving color bars with a variable speed and color.
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
#ifndef COLOR_BAR_ANIMATOR_H
#define COLOR_BAR_ANIMATOR_H

#include <vector>
#include "led/animator/LedAnimator.h"

namespace NL
{
	class ColorBarAnimator : public LedAnimator
	{
	public:
		enum class ColorBarMode
		{
			COLOR_BAR_LINEAR_HARD = 0,
			COLOR_BAR_LINEAR_SMOOTH = 1,
			COLOR_BAR_CENTER_HARD = 2,
			COLOR_BAR_CENTER_SMOOTH = 3
		};

		ColorBarAnimator();
		ColorBarAnimator(const NL::ColorBarAnimator::ColorBarMode colorBarMode, const NL::Pixel color1, const NL::Pixel color2);
		~ColorBarAnimator();

		void init(NL::LedStrip &ledStrip);
		void render(NL::LedStrip &ledStrip);

		void setColorBarMode(const NL::ColorBarAnimator::ColorBarMode colorBarMode);
		void setColor(const NL::Pixel color1, const NL::Pixel color2);

	private:
		float angle;
		NL::ColorBarAnimator::ColorBarMode colorBarMode;
		NL::Pixel color[2];
	};
}

#endif