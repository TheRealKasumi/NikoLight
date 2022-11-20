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
	this->gradientMode = TesLight::GradientAnimator::GradientMode::GRADIENT_LINEAR;
	this->color[0] = CRGB::Black;
	this->color[2] = CRGB::Black;
}

/**
 * @brief Create a new instance of {@link TesLight::GradientAnimator}.
 * @param gradientMode mode of the gradient
 * @param color1 first color value
 * @param color2 second color value
 */
TesLight::GradientAnimator::GradientAnimator(const TesLight::GradientAnimator::GradientMode gradientMode, const CRGB color1, const CRGB color2)
{
	this->gradientMode = gradientMode;
	this->color[0] = color1;
	this->color[1] = color2;
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
		this->pixels[i] = CRGB::Black;
	}
}

/**
 * @brief Render the gradient to the {@link TesLight::Pixel} array.
 */
void TesLight::GradientAnimator::render()
{
	if (this->pixelCount == 2)
	{
		const float middle = (this->offset / 255.0f - 0.5f) * 2.0f;
		if (middle < 0.0f)
		{
			this->pixels[0].setRGB(this->color[this->reverse ? 1 : 0].r, this->color[this->reverse ? 1 : 0].g, this->color[this->reverse ? 1 : 0].b);
			this->pixels[1].setRGB(
				(-middle * this->color[this->reverse ? 1 : 0].r + (1 + middle) * this->color[this->reverse ? 0 : 1].r),
				(-middle * this->color[this->reverse ? 1 : 0].g + (1 + middle) * this->color[this->reverse ? 0 : 1].g),
				(-middle * this->color[this->reverse ? 1 : 0].b + (1 + middle) * this->color[this->reverse ? 0 : 1].b));
		}
		else
		{
			this->pixels[0].setRGB(
				(middle * this->color[this->reverse ? 0 : 1].r + (1 - middle) * this->color[this->reverse ? 1 : 0].r),
				(middle * this->color[this->reverse ? 0 : 1].g + (1 - middle) * this->color[this->reverse ? 1 : 0].g),
				(middle * this->color[this->reverse ? 0 : 1].b + (1 - middle) * this->color[this->reverse ? 1 : 0].b));
			this->pixels[1].setRGB(this->color[this->reverse ? 0 : 1].r, this->color[this->reverse ? 0 : 1].g, this->color[this->reverse ? 0 : 1].b);
		}
	}
	else
	{
		float middle = (this->pixelCount - 1) * this->offset / 255.0f;
		if (middle < 0.01f)
		{
			middle = 0.01f;
		}
		else if (middle > this->pixelCount - 1.01f)
		{
			middle = this->pixelCount - 1.01f;
		}

		for (uint16_t i = 0; i < this->pixelCount; i++)
		{
			float position = 0.0f;
			if (this->gradientMode == TesLight::GradientAnimator::GradientMode::GRADIENT_LINEAR)
			{
				if (i < middle)
				{
					position = i * 0.5f / middle;
				}
				else
				{
					position = 0.5f + ((float)i - middle) / ((this->pixelCount - 1) - middle) * 0.5f;
				}
			}
			else if (this->gradientMode == TesLight::GradientAnimator::GradientMode::GRADIENT_CENTER)
			{
				if (i < middle)
				{
					position = (float)i / middle;
				}
				else
				{
					position = 1.0f - ((float)i - middle) / ((this->pixelCount - 1) - middle);
				}
			}
			this->pixels[i].setRGB(
				(position * this->color[this->reverse ? 1 : 0].r + (1 - position) * this->color[this->reverse ? 0 : 1].r),
				(position * this->color[this->reverse ? 1 : 0].g + (1 - position) * this->color[this->reverse ? 0 : 1].g),
				(position * this->color[this->reverse ? 1 : 0].b + (1 - position) * this->color[this->reverse ? 0 : 1].b));
		}
	}

	this->applyBrightness();
}

/**
 * @brief Set the gradient mode.
 * @param gradientMode mode of the gradient
 */
void TesLight::GradientAnimator::setGradientMode(const TesLight::GradientAnimator::GradientMode gradientMode)
{
	this->gradientMode = gradientMode;
}

/**
 * @brief Set the color for the gradient.
 * @param color1 first color value
 * @param color2 second color value
 */
void TesLight::GradientAnimator::setColor(const CRGB color1, const CRGB color2)
{
	this->color[0] = color1;
	this->color[1] = color2;
}