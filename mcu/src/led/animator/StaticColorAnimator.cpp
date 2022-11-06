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
 * @brief Create a new instance of {@link TesLight::StaticColorAnimator}.
 */
TesLight::StaticColorAnimator::StaticColorAnimator()
{
	this->color = CRGB::Black;
}

/**
 * @brief Create a new instance of {@link TesLight::StaticColorAnimator}.
 * @param color color to display
 */
TesLight::StaticColorAnimator::StaticColorAnimator(const CRGB color)
{
	this->color = color;
}

/**
 * @brief Destroy the {@link TesLight::StaticColorAnimator}.
 */
TesLight::StaticColorAnimator::~StaticColorAnimator()
{
}

/**
 * @brief Initialize the {@link StaticColorAnimator}.
 */
void TesLight::StaticColorAnimator::init()
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i] = CRGB::Black;
	}
}

/**
 * @brief Render the static color to the {@link TesLight::Pixel} array.
 */
void TesLight::StaticColorAnimator::render()
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i] = this->color;
	}

	this->applyBrightness();
}

/**
 * @brief Set the static color.
 * @param color color to display
 */
void TesLight::StaticColorAnimator::setColor(const CRGB color)
{
	this->color = color;
}