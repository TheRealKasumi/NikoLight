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
 * @brief Create a new instance of {@link TL::StaticColorAnimator}.
 * @param color color to display
 */
TL::StaticColorAnimator::StaticColorAnimator(const CRGB color)
{
	this->color = color;
}

/**
 * @brief Destroy the {@link TL::StaticColorAnimator}.
 */
TL::StaticColorAnimator::~StaticColorAnimator()
{
}

/**
 * @brief Initialize the {@link StaticColorAnimator}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::StaticColorAnimator::init(std::vector<CRGB> &pixels)
{
	std::fill(pixels.begin(), pixels.end(), CRGB::Black);
}

/**
 * @brief Render the static color to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::StaticColorAnimator::render(std::vector<CRGB> &pixels)
{
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i) = this->color;
	}

	this->applyBrightness(pixels);
}
