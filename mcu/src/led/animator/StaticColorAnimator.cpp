/**
 * @file StaticColorAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link StaticColorAnimator}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/StaticColorAnimator.h"

/**
 * @brief Create a new instance of {@link TL::StaticColorAnimator}.
 */
TL::StaticColorAnimator::StaticColorAnimator()
{
	this->color = CRGB::Black;
}

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
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i) = CRGB::Black;
	}
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

/**
 * @brief Set the static color.
 * @param color color to display
 */
void TL::StaticColorAnimator::setColor(const CRGB color)
{
	this->color = color;
}