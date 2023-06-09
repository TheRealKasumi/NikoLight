/**
 * @file EqualizerAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a moving bar based on audio data.
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
#ifndef EQUALIZER_ANIMATOR_H
#define EQUALIZER_ANIMATOR_H

#include "led/animator/LedAnimator.h"

namespace NL
{
	class EqualizerAnimator : public LedAnimator
	{
	public:
		EqualizerAnimator(const NL::Pixel color1, const NL::Pixel color2, const uint8_t rainbowSpeed, const float autoGain, const uint8_t frequencyBandMask);
		~EqualizerAnimator();

		void init(NL::LedStrip &ledStrip);
		void render(NL::LedStrip &ledStrip);

	private:
		NL::Pixel color[2];
		uint8_t rainbowSpeed;
		float autoGain;
		uint8_t frequencyBandMask;

		uint8_t audioSequence;
		float currentVolume;
		float peakVolume;
		float colorAngle;
		bool rainbowMode[2];
	};
}

#endif