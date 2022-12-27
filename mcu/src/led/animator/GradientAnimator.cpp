/**
 * @file GradientAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::GradientAnimator}.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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
 */
TL::GradientAnimator::GradientAnimator()
{
	this->gradientMode = TL::GradientAnimator::GradientMode::GRADIENT_LINEAR;
	this->color[0] = CRGB::Black;
	this->color[1] = CRGB::Black;
}

/**
 * @brief Create a new instance of {@link TL::GradientAnimator}.
 * @param gradientMode mode of the gradient
 * @param color1 first color value
 * @param color2 second color value
 */
TL::GradientAnimator::GradientAnimator(const TL::GradientAnimator::GradientMode gradientMode, const CRGB color1, const CRGB color2)
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
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::GradientAnimator::init(std::vector<CRGB> &pixels)
{
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i) = CRGB::Black;
	}
}

/**
 * @brief Render the gradient to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::GradientAnimator::render(std::vector<CRGB> &pixels)
{
	if (pixels.size() == 2)
	{
		const float middle = (this->offset / 255.0f - 0.5f) * 2.0f;
		if (middle < 0.0f)
		{
			pixels.at(0).setRGB(this->color[this->reverse ? 1 : 0].r, this->color[this->reverse ? 1 : 0].g, this->color[this->reverse ? 1 : 0].b);
			pixels.at(1).setRGB(
				(-middle * this->color[this->reverse ? 1 : 0].r + (1 + middle) * this->color[this->reverse ? 0 : 1].r),
				(-middle * this->color[this->reverse ? 1 : 0].g + (1 + middle) * this->color[this->reverse ? 0 : 1].g),
				(-middle * this->color[this->reverse ? 1 : 0].b + (1 + middle) * this->color[this->reverse ? 0 : 1].b));
		}
		else
		{
			pixels.at(0).setRGB(
				(middle * this->color[this->reverse ? 0 : 1].r + (1 - middle) * this->color[this->reverse ? 1 : 0].r),
				(middle * this->color[this->reverse ? 0 : 1].g + (1 - middle) * this->color[this->reverse ? 1 : 0].g),
				(middle * this->color[this->reverse ? 0 : 1].b + (1 - middle) * this->color[this->reverse ? 1 : 0].b));
			pixels.at(1).setRGB(this->color[this->reverse ? 0 : 1].r, this->color[this->reverse ? 0 : 1].g, this->color[this->reverse ? 0 : 1].b);
		}
	}
	else
	{
		float middle = (pixels.size() - 1) * this->offset / 255.0f;
		if (middle < 0.01f)
		{
			middle = 0.01f;
		}
		else if (middle > pixels.size() - 1.01f)
		{
			middle = pixels.size() - 1.01f;
		}

		for (size_t i = 0; i < pixels.size(); i++)
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
					position = 0.5f + ((float)i - middle) / ((pixels.size() - 1) - middle) * 0.5f;
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
					position = 1.0f - ((float)i - middle) / ((pixels.size() - 1) - middle);
				}
			}
			pixels.at(i).setRGB(
				(position * this->color[this->reverse ? 1 : 0].r + (1 - position) * this->color[this->reverse ? 0 : 1].r),
				(position * this->color[this->reverse ? 1 : 0].g + (1 - position) * this->color[this->reverse ? 0 : 1].g),
				(position * this->color[this->reverse ? 1 : 0].b + (1 - position) * this->color[this->reverse ? 0 : 1].b));
		}
	}

	this->applyBrightness(pixels);
}

/**
 * @brief Set the gradient mode.
 * @param gradientMode mode of the gradient
 */
void TL::GradientAnimator::setGradientMode(const TL::GradientAnimator::GradientMode gradientMode)
{
	this->gradientMode = gradientMode;
}

/**
 * @brief Set the color for the gradient.
 * @param color1 first color value
 * @param color2 second color value
 */
void TL::GradientAnimator::setColor(const CRGB color1, const CRGB color2)
{
	this->color[0] = color1;
	this->color[1] = color2;
}