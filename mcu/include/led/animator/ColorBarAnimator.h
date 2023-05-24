/**
 * @file ColorBarAnimator.h
 * @author TheRealKasumi
 * @brief Contains a class to render moving color bars with a variable speed and color.
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