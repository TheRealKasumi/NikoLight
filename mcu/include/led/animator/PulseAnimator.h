/**
 * @file PulseAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render a pulsing color animation.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
 *
 * This project, including hardware and software, is provided "as is". There is no warranty
 * of any kind, express or implied, including but not limited to the warranties of fitness
 * for a particular purpose and noninfringement. TheRealKasumi (https://github.com/TheRealKasumi)
 * is holding ownership of this project. You are free to use, modify, distribute and contribute
 * to this project for private, non-commercial purposes. It is granted to include this hardware
 * and software into private, non-commercial projects. However, the source code of any project,
 * software and hardware that is including this project must be public and free to use for private
 * persons. Any commercial use is hereby strictly prohibited without agreement from the owner.
 * By contributing to the project, you agree that the ownership of your work is transferred to
 * the project owner and that you lose any claim to your contribute work. This copyright and
 * license note applies to all files of this project and must not be removed without agreement
 * from the owner.
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