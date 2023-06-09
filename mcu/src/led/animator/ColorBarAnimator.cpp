/**
 * @file ColorBarAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@NL::ColorBarAnimator}.
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
#include "led/animator/ColorBarAnimator.h"

/**
 * @brief Create a new instance of {@link NL::ColorBarAnimator}.
 */
NL::ColorBarAnimator::ColorBarAnimator()
{
	this->angle = 0.0f;
	this->colorBarMode = NL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD;
	this->color[0] = NL::Pixel::ColorCode::Black;
	this->color[1] = NL::Pixel::ColorCode::Black;
}

/**
 * @brief Create a new instance of {@link NL::ColorBarAnimator}.
 *
 * @param colorBarMode display mode of the color bars
 * @param color1 first color of the bars
 * @param color2 second color of the bars
 */
NL::ColorBarAnimator::ColorBarAnimator(const NL::ColorBarAnimator::ColorBarMode colorBarMode, const NL::Pixel color1, const NL::Pixel color2)
{
	this->colorBarMode = colorBarMode;
	this->color[0] = color1;
	this->color[1] = color2;
}

/**
 * @brief Destroy the {@link NL::ColorBarAnimator} instance.
 */
NL::ColorBarAnimator::~ColorBarAnimator()
{
}

/**
 * @brief Initialize the {@link NL::ColorBarAnimator}.
 * @param ledStrip LED strip with the pixel data
 */
void NL::ColorBarAnimator::init(NL::LedStrip &ledStrip)
{
	this->angle = 0.0f;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the color bars to the vector holding the LED pixel data
 * @param ledStrip LED strip with the pixel data
 */
void NL::ColorBarAnimator::render(NL::LedStrip &ledStrip)
{
	const float middle = ledStrip.getLedCount() / 2;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		float colorAngle1 = 0.0f;
		float colorAngle2 = 0.0f;
		float offset = this->offset / 5.0f;

		if (this->colorBarMode == NL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD || this->colorBarMode == NL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_SMOOTH)
		{
			colorAngle1 = (this->angle + 0.0f) + i * offset;
			colorAngle2 = (this->angle + 180.0f) + i * offset;
		}

		else if (this->colorBarMode == NL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_HARD || this->colorBarMode == NL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_SMOOTH)
		{
			colorAngle1 = i < middle ? (this->angle + 0.0f) + i * offset : (this->angle + 0.0f) + (ledStrip.getLedCount() - i) * offset;
			colorAngle2 = i < middle ? (this->angle + 180.0f) + i * offset : (this->angle + 180.0f) + (ledStrip.getLedCount() - i) * offset;
		}

		float trapezoidValue1 = this->trapezoid2(colorAngle1);
		float trapezoidValue2 = this->trapezoid2(colorAngle2);

		if (this->colorBarMode == NL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD || this->colorBarMode == NL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_HARD)
		{
			trapezoidValue1 = trapezoidValue1 < 0.5f ? 0.0f : 1.0f;
			trapezoidValue2 = trapezoidValue2 < 0.5f ? 0.0f : 1.0f;
		}

		ledStrip.setPixel(
			NL::Pixel(
				trapezoidValue1 * this->color[0].red + trapezoidValue2 * this->color[1].red,
				trapezoidValue1 * this->color[0].green + trapezoidValue2 * this->color[1].green,
				trapezoidValue1 * this->color[0].blue + trapezoidValue2 * this->color[1].blue),
			i);
	}

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

	this->applyBrightness(ledStrip);
}

/**
 * @brief Set the mode of the color bar animation.
 * @param ColorBarMode mode of the animation
 */
void NL::ColorBarAnimator::setColorBarMode(const NL::ColorBarAnimator::ColorBarMode colorBarMode)
{
	this->colorBarMode = colorBarMode;
}

/**
 * @brief Set the color for the color bars.
 * @param color1 first color of the bars
 * @param color2 second color of the bars
 */
void NL::ColorBarAnimator::setColor(const NL::Pixel color1, const NL::Pixel color2)
{
	this->color[0] = color1;
	this->color[1] = color2;
}
