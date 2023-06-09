/**
 * @file GradientAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::GradientAnimator}.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "led/animator/GradientAnimator.h"

/**
 * @brief Create a new instance of {@link NL::GradientAnimator}.
 * @param gradientMode mode of the gradient
 * @param color1 first color value
 * @param color2 second color value
 */
NL::GradientAnimator::GradientAnimator(const NL::GradientAnimator::GradientMode gradientMode, const NL::Pixel color1, const NL::Pixel color2)
{
	this->gradientMode = gradientMode;
	this->color[0] = color1;
	this->color[1] = color2;
}

/**
 * @brief Destroy the {@link NL::GradientAnimator}.
 */
NL::GradientAnimator::~GradientAnimator()
{
}

/**
 * @brief Initialize the {@link NL::GradientAnimator}.
 * @param ledStrip LED strip with the pixel data
 */
void NL::GradientAnimator::init(NL::LedStrip &ledStrip)
{
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the gradient to the vector holding the LED pixel data
 * @param ledStrip LED strip with the pixel data
 */
void NL::GradientAnimator::render(NL::LedStrip &ledStrip)
{
	if (ledStrip.getLedCount() == 2)
	{
		const float middle = (this->offset / 255.0f - 0.5f) * 2.0f;
		if (middle < 0.0f)
		{
			ledStrip.setPixel(NL::Pixel(this->color[1].red, this->color[1].green, this->color[1].blue), 0);
			ledStrip.setPixel(
				NL::Pixel(
					(-middle * this->color[1].red + (1 + middle) * this->color[0].red),
					(-middle * this->color[1].green + (1 + middle) * this->color[0].green),
					(-middle * this->color[1].blue + (1 + middle) * this->color[0].blue)),
				1);
		}
		else
		{
			ledStrip.setPixel(
				NL::Pixel(
					(middle * this->color[0].red + (1 - middle) * this->color[1].red),
					(middle * this->color[0].green + (1 - middle) * this->color[1].green),
					(middle * this->color[0].blue + (1 - middle) * this->color[1].blue)),
				0);
			ledStrip.setPixel(NL::Pixel(this->color[0].red, this->color[0].green, this->color[0].blue), 1);
		}
	}
	else
	{
		float middle = (ledStrip.getLedCount() - 1) * this->offset / 255.0f;
		if (middle < 0.01f)
		{
			middle = 0.01f;
		}
		else if (middle > ledStrip.getLedCount() - 1.01f)
		{
			middle = ledStrip.getLedCount() - 1.01f;
		}

		for (size_t i = 0; i < ledStrip.getLedCount(); i++)
		{
			float position = 0.0f;
			if (this->gradientMode == NL::GradientAnimator::GradientMode::GRADIENT_LINEAR)
			{
				if (i < middle)
				{
					position = i * 0.5f / middle;
				}
				else
				{
					position = 0.5f + (i - middle) / ((ledStrip.getLedCount() - 1) - middle) * 0.5f;
				}
			}
			else if (this->gradientMode == NL::GradientAnimator::GradientMode::GRADIENT_CENTER)
			{
				if (i < middle)
				{
					position = i / middle;
				}
				else
				{
					position = 1.0f - (i - middle) / ((ledStrip.getLedCount() - 1) - middle);
				}
			}
			ledStrip.setPixel(
				NL::Pixel(
					(position * this->color[1].red + (1 - position) * this->color[0].red),
					(position * this->color[1].green + (1 - position) * this->color[0].green),
					(position * this->color[1].blue + (1 - position) * this->color[0].blue)),
				i);
		}
	}

	if (this->reverse)
	{
		this->reversePixels(ledStrip);
	}
	this->applyBrightness(ledStrip);
}
