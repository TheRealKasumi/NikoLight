/**
 * @file StaticColorAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link StaticColorAnimator}.
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
#include "led/animator/StaticColorAnimator.h"

/**
 * @brief Create a new instance of {@link NL::StaticColorAnimator}.
 * @param color color to display
 */
NL::StaticColorAnimator::StaticColorAnimator(const NL::Pixel color)
{
	this->color = color;
}

/**
 * @brief Destroy the {@link NL::StaticColorAnimator}.
 */
NL::StaticColorAnimator::~StaticColorAnimator()
{
}

/**
 * @brief Initialize the {@link StaticColorAnimator}.
 * @param ledStrip LED strip with the pixel data
 */
void NL::StaticColorAnimator::init(NL::LedStrip &ledStrip)
{
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the static color to the vector holding the LED pixel data
 * @param ledStrip LED strip with the pixel data
 */
void NL::StaticColorAnimator::render(NL::LedStrip &ledStrip)
{
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(this->color, i);
	}

	this->applyBrightness(ledStrip);
}
