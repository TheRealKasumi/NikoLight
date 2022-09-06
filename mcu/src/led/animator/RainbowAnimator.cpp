/**
 * @file RainbowAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TesLight::RainbowAnimator}.
 * @version 0.0.1
 * @date 2022-09-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/RainbowAnimator.h"

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
 */
void TesLight::RainbowAnimator::render()
{
	const uint16_t middle = this->pixelCount / 2;
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		float redAngle = 0.0f;
		float greenAngle = 0.0f;
		float blueAngle = 0.0f;

		if (this->rainbowMode == TesLight::RainbowMode::RAINBOW_SOLID)
		{
			redAngle = this->angle + 0.0f;
			greenAngle = this->angle + 120.0f;
			blueAngle = this->angle + 240.0f;
		}

		else if (this->rainbowMode == TesLight::RainbowMode::RAINBOW_LINEAR)
		{
			redAngle = (this->angle + 0.0f) + i * this->offset;
			greenAngle = (this->angle + 120.0f) + i * this->offset;
			blueAngle = (this->angle + 240.0f) + i * this->offset;
		}

		else if (this->rainbowMode == TesLight::RainbowMode::RAINBOW_CENTER)
		{
			redAngle = i < middle ? (this->angle + 0.0f) + i * this->offset : (this->angle + 0.0f) + (this->pixelCount - i) * this->offset;
			greenAngle = i < middle ? (this->angle + 120.0f) + i * this->offset : (this->angle + 120.0f) + (this->pixelCount - i) * this->offset;
			blueAngle = i < middle ? (this->angle + 240.0f) + i * this->offset : (this->angle + 240.0f) + (this->pixelCount - i) * this->offset;
		}

		this->pixels[i].setRed(this->trapezoid(redAngle) * 255.0f * this->brightness);
		this->pixels[i].setGreen(this->trapezoid(greenAngle) * 255.0f * this->brightness);
		this->pixels[i].setBlue(this->trapezoid(blueAngle) * 255.0f * this->brightness);
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
