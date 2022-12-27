/**
 * @file RainbowAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TL::RainbowAnimator}.
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
#include "led/animator/RainbowAnimator.h"

/**
 * @brief Create a new instance of {@link TL::RainbowAnimator}.
 */
TL::RainbowAnimator::RainbowAnimator()
{
	this->angle = 0.0f;
	this->rainbowMode = TL::RainbowAnimator::RainbowMode::RAINBOW_SOLID;
}

/**
 * @brief Create a new instance of {@link TL::RainbowAnimator}.
 *
 * @param rainbowMode display mode of the rainbow
 */
TL::RainbowAnimator::RainbowAnimator(const TL::RainbowAnimator::RainbowMode rainbowMode)
{
	this->angle = 0.0f;
	this->rainbowMode = rainbowMode;
}

/**
 * @brief Destroy the {@link TL::RainbowAnimator} instance.
 */
TL::RainbowAnimator::~RainbowAnimator()
{
}

/**
 * @brief Initialize the {@link TL::RainbowAnimator}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::RainbowAnimator::init(std::vector<CRGB> &pixels)
{
	this->angle = 0.0f;
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i) = CRGB::Black;
	}
}

/**
 * @brief Render a rainbow to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::RainbowAnimator::render(std::vector<CRGB> &pixels)
{
	const float middle = pixels.size() / 2;
	for (size_t i = 0; i < pixels.size(); i++)
	{
		float redAngle = 0.0f;
		float greenAngle = 0.0f;
		float blueAngle = 0.0f;
		const float offset = this->offset / 25.0f;

		if (this->rainbowMode == TL::RainbowAnimator::RainbowMode::RAINBOW_SOLID)
		{
			redAngle = this->angle + 0.0f;
			greenAngle = this->angle + 120.0f;
			blueAngle = this->angle + 240.0f;
		}

		else if (this->rainbowMode == TL::RainbowAnimator::RainbowMode::RAINBOW_LINEAR)
		{
			redAngle = (this->angle + 0.0f) + i * offset;
			greenAngle = (this->angle + 120.0f) + i * offset;
			blueAngle = (this->angle + 240.0f) + i * offset;
		}

		else if (this->rainbowMode == TL::RainbowAnimator::RainbowMode::RAINBOW_CENTER)
		{
			redAngle = i < middle ? (this->angle + 0.0f) + i * offset : (this->angle + 0.0f) + (pixels.size() - i) * offset;
			greenAngle = i < middle ? (this->angle + 120.0f) + i * offset : (this->angle + 120.0f) + (pixels.size() - i) * offset;
			blueAngle = i < middle ? (this->angle + 240.0f) + i * offset : (this->angle + 240.0f) + (pixels.size() - i) * offset;
		}

		pixels.at(i).setRGB(this->trapezoid(redAngle) * 255.0f, this->trapezoid(greenAngle) * 255.0f, this->trapezoid(blueAngle) * 255.0f);
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
 * @brief Set the mode of the rainbow animation.
 * @param rainbowMode mode of the animation
 */
void TL::RainbowAnimator::setRainbowMode(const TL::RainbowAnimator::RainbowMode rainbowMode)
{
	this->rainbowMode = rainbowMode;
}
