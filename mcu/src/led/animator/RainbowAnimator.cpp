/**
 * @file RainbowAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TesLight::RainbowAnimator}.
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
	this->rainbowMode = TesLight::RainbowAnimator::RainbowMode::RAINBOW_SOLID;
}

/**
 * @brief Construct a new Tes Light:: Rainbow Animator:: Rainbow Animator object
 *
 * @param rainbowMode display mode of the rainbow
 */
TesLight::RainbowAnimator::RainbowAnimator(const TesLight::RainbowAnimator::RainbowMode rainbowMode)
{
	this->rainbowMode = rainbowMode;
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
		this->pixels[i] = CRGB::Black;
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
		float offset = this->offset / 50.0f;

		if (this->rainbowMode == TesLight::RainbowAnimator::RainbowMode::RAINBOW_SOLID)
		{
			redAngle = this->angle + 0.0f;
			greenAngle = this->angle + 120.0f;
			blueAngle = this->angle + 240.0f;
		}

		else if (this->rainbowMode == TesLight::RainbowAnimator::RainbowMode::RAINBOW_LINEAR)
		{
			redAngle = (this->angle + 0.0f) + i * offset;
			greenAngle = (this->angle + 120.0f) + i * offset;
			blueAngle = (this->angle + 240.0f) + i * offset;
		}

		else if (this->rainbowMode == TesLight::RainbowAnimator::RainbowMode::RAINBOW_CENTER)
		{
			redAngle = i < middle ? (this->angle + 0.0f) + i * offset : (this->angle + 0.0f) + (this->pixelCount - i) * offset;
			greenAngle = i < middle ? (this->angle + 120.0f) + i * offset : (this->angle + 120.0f) + (this->pixelCount - i) * offset;
			blueAngle = i < middle ? (this->angle + 240.0f) + i * offset : (this->angle + 240.0f) + (this->pixelCount - i) * offset;
		}

		this->pixels[i].setRGB(this->trapezoid(redAngle) * 255.0f, this->trapezoid(greenAngle) * 255.0f, this->trapezoid(blueAngle) * 255.0f);
	}

	this->applyBrightness();

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
void TesLight::RainbowAnimator::setRainbowMode(const TesLight::RainbowAnimator::RainbowMode rainbowMode)
{
	this->rainbowMode = rainbowMode;
}
