/**
 * @file StaticColorAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link StaticColorAnimator}.
 * @version 0.0.1
 * @date 2022-06-28
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
	this->red = 0;
	this->green = 0;
	this->blue = 0;
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
		this->pixels[i].setColor(0);
	}
}

/**
 * @brief Render the static color to the {@link TesLight::Pixel} array.
 */
void TesLight::StaticColorAnimator::render()
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i].setRed(this->red);
		this->pixels[i].setGreen(this->green);
		this->pixels[i].setBlue(this->blue);
	}
}

/**
 * @brief Set the static color.
 * @param red red value
 * @param green green value
 * @param blue blue value
 */
void TesLight::StaticColorAnimator::setColor(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	this->red = red;
	this->green = green;
	this->blue = blue;
}