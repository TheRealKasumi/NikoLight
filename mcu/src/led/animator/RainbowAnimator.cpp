/**
 * @file RainbowAnimator.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@NL::RainbowAnimator}.
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
#include "led/animator/RainbowAnimator.h"

/**
 * @brief Create a new instance of {@link NL::RainbowAnimator}.
 *
 * @param rainbowMode display mode of the rainbow
 */
NL::RainbowAnimator::RainbowAnimator(const NL::RainbowAnimator::RainbowMode rainbowMode)
{
	this->angle = 0.0f;
	this->rainbowMode = rainbowMode;
}

/**
 * @brief Destroy the {@link NL::RainbowAnimator} instance.
 */
NL::RainbowAnimator::~RainbowAnimator()
{
}

/**
 * @brief Initialize the {@link NL::RainbowAnimator}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void NL::RainbowAnimator::init(NL::LedStrip &ledStrip)
{
	this->angle = 0.0f;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render a rainbow to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void NL::RainbowAnimator::render(NL::LedStrip &ledStrip)
{
	const float middle = ledStrip.getLedCount() / 2;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		float redAngle = 0.0f;
		float greenAngle = 0.0f;
		float blueAngle = 0.0f;
		const float offset = this->offset / 25.0f;

		if (this->rainbowMode == NL::RainbowAnimator::RainbowMode::RAINBOW_SOLID)
		{
			redAngle = this->angle + 0.0f;
			greenAngle = this->angle + 120.0f;
			blueAngle = this->angle + 240.0f;
		}

		else if (this->rainbowMode == NL::RainbowAnimator::RainbowMode::RAINBOW_LINEAR)
		{
			redAngle = (this->angle + 0.0f) + i * offset;
			greenAngle = (this->angle + 120.0f) + i * offset;
			blueAngle = (this->angle + 240.0f) + i * offset;
		}

		else if (this->rainbowMode == NL::RainbowAnimator::RainbowMode::RAINBOW_CENTER)
		{
			redAngle = i < middle ? (this->angle + 0.0f) + i * offset : (this->angle + 0.0f) + (ledStrip.getLedCount() - i) * offset;
			greenAngle = i < middle ? (this->angle + 120.0f) + i * offset : (this->angle + 120.0f) + (ledStrip.getLedCount() - i) * offset;
			blueAngle = i < middle ? (this->angle + 240.0f) + i * offset : (this->angle + 240.0f) + (ledStrip.getLedCount() - i) * offset;
		}

		ledStrip.setPixel(NL::Pixel(this->trapezoid(redAngle) * 255.0f, this->trapezoid(greenAngle) * 255.0f, this->trapezoid(blueAngle) * 255.0f), i);
	}

	this->applyBrightness(ledStrip);

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
