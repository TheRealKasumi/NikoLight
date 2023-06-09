/**
 * @file StaticColorAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a static color
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
#ifndef STATIC_COLOR_ANIMATOR_H
#define STATIC_COLOR_ANIMATOR_H

#include <vector>
#include "led/animator/LedAnimator.h"

namespace NL
{
	class StaticColorAnimator : public LedAnimator
	{
	public:
		StaticColorAnimator(const NL::Pixel color);
		~StaticColorAnimator();

		void init(NL::LedStrip &ledStrip);
		void render(NL::LedStrip &ledStrip);

	private:
		NL::Pixel color;
	};
}

#endif
