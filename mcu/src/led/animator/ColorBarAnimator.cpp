/**
 * @file ColorBarAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TL::ColorBarAnimator}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/ColorBarAnimator.h"

/**
 * @brief Create a new instance of {@link TL::ColorBarAnimator}.
 */
TL::ColorBarAnimator::ColorBarAnimator()
{
	this->angle = 0.0f;
	this->colorBarMode = TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD;
	this->color[0] = CRGB::Black;
	this->color[1] = CRGB::Black;
}

/**
 * @brief Create a new instance of {@link TL::ColorBarAnimator}.
 *
 * @param colorBarMode display mode of the color bars
 * @param color1 first color of the bars
 * @param color2 second color of the bars
 */
TL::ColorBarAnimator::ColorBarAnimator(const TL::ColorBarAnimator::ColorBarMode colorBarMode, const CRGB color1, const CRGB color2)
{
	this->colorBarMode = colorBarMode;
	this->color[0] = color1;
	this->color[1] = color2;
}

/**
 * @brief Destroy the {@link TL::ColorBarAnimator} instance.
 */
TL::ColorBarAnimator::~ColorBarAnimator()
{
}

/**
 * @brief Initialize the {@link TL::ColorBarAnimator}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::ColorBarAnimator::init(std::vector<CRGB> &pixels)
{
	this->angle = 0.0f;
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i) = CRGB::Black;
	}
}

/**
 * @brief Render the color bars to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::ColorBarAnimator::render(std::vector<CRGB> &pixels)
{
	const float middle = pixels.size() / 2;
	for (size_t i = 0; i < pixels.size(); i++)
	{
		float colorAngle1 = 0.0f;
		float colorAngle2 = 0.0f;
		float offset = this->offset / 5.0f;

		if (this->colorBarMode == TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD || this->colorBarMode == TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_SMOOTH)
		{
			colorAngle1 = (this->angle + 0.0f) + i * offset;
			colorAngle2 = (this->angle + 180.0f) + i * offset;
		}

		else if (this->colorBarMode == TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_HARD || this->colorBarMode == TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_SMOOTH)
		{
			colorAngle1 = i < middle ? (this->angle + 0.0f) + i * offset : (this->angle + 0.0f) + (pixels.size() - i) * offset;
			colorAngle2 = i < middle ? (this->angle + 180.0f) + i * offset : (this->angle + 180.0f) + (pixels.size() - i) * offset;
		}

		float trapezoidValue1 = this->trapezoid2(colorAngle1);
		float trapezoidValue2 = this->trapezoid2(colorAngle2);

		if (this->colorBarMode == TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD || this->colorBarMode == TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_HARD)
		{
			trapezoidValue1 = trapezoidValue1 < 0.5f ? 0.0f : 1.0f;
			trapezoidValue2 = trapezoidValue2 < 0.5f ? 0.0f : 1.0f;
		}

		pixels.at(i).setRGB(
			trapezoidValue1 * this->color[0].r + trapezoidValue2 * this->color[1].r,
			trapezoidValue1 * this->color[0].g + trapezoidValue2 * this->color[1].g,
			trapezoidValue1 * this->color[0].b + trapezoidValue2 * this->color[1].b);
	}

	this->applyBrightness(pixels);

	if (this->reverse)
	{
		this->angle += this->speed / 50.0f;
	}
	else
	{
		this->angle -= this->speed / 50.0f;
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
 * @brief Set the mode of the color bar animation.
 * @param ColorBarMode mode of the animation
 */
void TL::ColorBarAnimator::setColorBarMode(const TL::ColorBarAnimator::ColorBarMode colorBarMode)
{
	this->colorBarMode = colorBarMode;
}

/**
 * @brief Set the color for the color bars.
 * @param color1 first color of the bars
 * @param color2 second color of the bars
 */
void TL::ColorBarAnimator::setColor(const CRGB color1, const CRGB color2)
{
	this->color[0] = color1;
	this->color[1] = color2;
}
