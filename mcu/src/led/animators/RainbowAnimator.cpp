/**
 * @file RainbowAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TesLight::RainbowAnimator}.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animators/RainbowAnimator.h"

/**
 * @brief Create a new instance of {@link TesLight::RainbowAnimator}.
 */
TesLight::RainbowAnimator::RainbowAnimator()
{
	this->angle = 0.0f;
}

/**
 * @brief Destroy the {@link TesLight::RainbowAnimator} instance.
 */
TesLight::RainbowAnimator::~RainbowAnimator()
{
}

/**
 * @brief Initialize the {@link TesLight::RainbowAnimator}.
 */
void TesLight::RainbowAnimator::init()
{
	this->angle = 0.0f;
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i].setColor(0);
	}
}

/**
 * @brief Render a rainbow to the {@link TesLight::Pixel} array.
 *        Depending on the settings this can be done from side to side or from the middle.
 */
void TesLight::RainbowAnimator::render()
{
	const uint16_t middle = this->pixelCount / 2;
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		const float red = this->rainbowMode == TesLight::RainbowMode::RAINBOW_LINEAR || i < middle ? this->degToRad((angle + 0.0f) + i * this->offset) : this->degToRad((360.0f - angle + 0.0f) + i * this->offset);
		const float green = this->rainbowMode == TesLight::RainbowMode::RAINBOW_LINEAR || i < middle ? this->degToRad((angle + 120.0f) + i * this->offset) : this->degToRad((360.0f - angle + 120.0f) + i * this->offset);
		const float blue = this->rainbowMode == TesLight::RainbowMode::RAINBOW_LINEAR || i < middle ? this->degToRad((angle + 240.0f) + i * this->offset) : this->degToRad((360.0f - angle + 240.0f) + i * this->offset);

		this->pixels[i].setRed(this->cutNegative(fastCos(red)) * 255.0f * this->brightness);
		this->pixels[i].setGreen(this->cutNegative(fastCos(green)) * 255.0f * this->brightness);
		this->pixels[i].setBlue(this->cutNegative(fastCos(blue)) * 255.0f * this->brightness);
	}

	if (this->reverse)
	{
		this->angle += this->speed / 51.0f;
	}
	else
	{
		this->angle -= this->speed / 51.0f;
	}

	if (this->angle >= 360.0f)
	{
		this->angle -= 360.0f;
	}
	else if (this->angle < 0.0f)
	{
		this->angle += 360.0f;
	}
}

/**
 * @brief Set the mode of the rainbow animation.
 * @param rainbowMode mode of the animation
 */
void TesLight::RainbowAnimator::setRainbowMode(const TesLight::RainbowMode rainbowMode)
{
	this->rainbowMode = rainbowMode;
}
