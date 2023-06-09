/**
 * @file PulseAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a pulsing color animation.
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
#ifndef PULSE_ANIMATOR_H
#define PULSE_ANIMATOR_H

#include "led/animator/LedAnimator.h"

namespace NL
{
	class PulseAnimator : public LedAnimator
	{
	public:
		enum class PulseMode : uint8_t
		{
			LINEAR = 0,	   // Linear mode
			NON_LINEAR = 1 // Non-Linear mode
		};

		PulseAnimator(const PulseMode pulseMode, const NL::Pixel color, const float pulseFading, const uint8_t frequencyBandMask);
		~PulseAnimator();

		void init(NL::LedStrip &ledStrip);
		void render(NL::LedStrip &ledStrip);

	private:
		NL::Pixel color;
		float pulseFading;
		NL::PulseAnimator::PulseMode pulseMode;
		uint8_t frequencyBandMask;

		uint8_t mode;
		float pulseBrightness;
		float colorAngle;
		uint8_t audioSequence;
	};
}

#endif