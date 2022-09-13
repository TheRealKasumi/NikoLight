/**
 * @file GradientAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::GradientAnimator}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/GradientAnimator.h"

/**
 * @brief Create a new instance of {@link TesLight::GradientAnimator}.
 */
TesLight::GradientAnimator::GradientAnimator()
{
	for (uint8_t i = 0; i < 2; i++)
	{
		this->red[i] = 0;
		this->green[i] = 0;
		this->blue[i] = 0;
	}
}

/**
 * @brief Destroy the {@link TesLight::GradientAnimator}.
 */
TesLight::GradientAnimator::~GradientAnimator()
{
}

/**
 * @brief Initialize the {@link TesLight::GradientAnimator}.
 */
void TesLight::GradientAnimator::init()
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i].setColor(0);
	}
}

/**
 * @brief Render the gradient to the {@link TesLight::Pixel} array.
 */
void TesLight::GradientAnimator::render()
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		const float position = (float)i / this->pixelCount;
		this->pixels[i].setRed((position * this->red[this->reverse ? 0 : 1] + (1 - position) * red[this->reverse ? 1 : 0]));
		this->pixels[i].setGreen((position * this->green[this->reverse ? 0 : 1] + (1 - position) * green[this->reverse ? 1 : 0]));
		this->pixels[i].setBlue((position * this->blue[this->reverse ? 0 : 1] + (1 - position) * blue[this->reverse ? 1 : 0]));
	}
}

/**
 * @brief Set the color for the gradient.
 * @param red1 rest start
 * @param green1 green start
 * @param blue1 blue start
 * @param red2 red end
 * @param green2 greed end
 * @param blue2 blue end
 */
void TesLight::GradientAnimator::setColor(const uint8_t red1, const uint8_t green1, const uint8_t blue1, const uint8_t red2, const uint8_t green2, const uint8_t blue2)
{
	this->red[0] = red1;
	this->red[1] = red2;
	this->green[0] = green1;
	this->green[1] = green2;
	this->blue[0] = blue1;
	this->blue[1] = blue2;
}