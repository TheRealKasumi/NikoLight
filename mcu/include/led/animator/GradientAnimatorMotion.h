/**
 * @file GradientAnimatorMotion.h
 * @author TheRealKasumi
 * @brief Contains a class to render a gradient based on the cars motion.
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
#ifndef GRADIENT_ANIMATOR_MOTION_H
#define GRADIENT_ANIMATOR_MOTION_H

#include <vector>
#include "led/animator/LedAnimator.h"
#include "sensor/MotionSensor.h"

namespace TL
{
	class GradientAnimatorMotion : public LedAnimator
	{
	public:
		enum class GradientMode : uint8_t
		{
			GRADIENT_LINEAR = 0,
			GRADIENT_CENTER = 1
		};

		GradientAnimatorMotion(const TL::GradientAnimatorMotion::GradientMode gradientMode, const TL::Pixel color1, const TL::Pixel color2);
		~GradientAnimatorMotion();

		void init(TL::LedStrip &ledStrip);
		void render(TL::LedStrip &ledStrip);

	private:
		TL::GradientAnimatorMotion::GradientMode gradientMode;
		TL::Pixel color[2];

		float getMotionOffset();
	};
}

#endif