/**
 * @file GradientAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::GradientAnimator}.
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
#include "led/animator/GradientAnimator.h"

/**
 * @brief Create a new instance of {@link TL::GradientAnimator}.
 * @param gradientMode mode of the gradient
 * @param color1 first color value
 * @param color2 second color value
 */
TL::GradientAnimator::GradientAnimator(const TL::GradientAnimator::GradientMode gradientMode, const TL::Pixel color1, const TL::Pixel color2)
{
	this->gradientMode = gradientMode;
	this->color[0] = color1;
	this->color[1] = color2;
}

/**
 * @brief Destroy the {@link TL::GradientAnimator}.
 */
TL::GradientAnimator::~GradientAnimator()
{
}

/**
 * @brief Initialize the {@link TL::GradientAnimator}.
 * @param ledStrip LED strip with the pixel data
 */
void TL::GradientAnimator::init(TL::LedStrip &ledStrip)
{
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(TL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the gradient to the vector holding the LED pixel data
 * @param ledStrip LED strip with the pixel data
 */
void TL::GradientAnimator::render(TL::LedStrip &ledStrip)
{
	if (ledStrip.getLedCount() == 2)
	{
		const float middle = (this->offset / 255.0f - 0.5f) * 2.0f;
		if (middle < 0.0f)
		{
			ledStrip.setPixel(TL::Pixel(this->color[1].red, this->color[1].green, this->color[1].blue), 0);
			ledStrip.setPixel(
				TL::Pixel(
					(-middle * this->color[1].red + (1 + middle) * this->color[0].red),
					(-middle * this->color[1].green + (1 + middle) * this->color[0].green),
					(-middle * this->color[1].blue + (1 + middle) * this->color[0].blue)),
				1);
		}
		else
		{
			ledStrip.setPixel(
				TL::Pixel(
					(middle * this->color[0].red + (1 - middle) * this->color[1].red),
					(middle * this->color[0].green + (1 - middle) * this->color[1].green),
					(middle * this->color[0].blue + (1 - middle) * this->color[1].blue)),
				0);
			ledStrip.setPixel(TL::Pixel(this->color[0].red, this->color[0].green, this->color[0].blue), 1);
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
			if (this->gradientMode == TL::GradientAnimator::GradientMode::GRADIENT_LINEAR)
			{
				if (i < middle)
				{
					position = i * 0.5f / middle;
				}
				else
				{
					position = 0.5f + ((float)i - middle) / ((ledStrip.getLedCount() - 1) - middle) * 0.5f;
				}
			}
			else if (this->gradientMode == TL::GradientAnimator::GradientMode::GRADIENT_CENTER)
			{
				if (i < middle)
				{
					position = (float)i / middle;
				}
				else
				{
					position = 1.0f - ((float)i - middle) / ((ledStrip.getLedCount() - 1) - middle);
				}
			}
			ledStrip.setPixel(
				TL::Pixel(
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
