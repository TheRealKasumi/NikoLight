/**
 * @file Pixel.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::Pixel} class.
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
#include "led/driver/Pixel.h"

/**
 * @brief Create a new instance of {@link NL::Pixel}.
 */
NL::Pixel::Pixel()
{
	this->setColor(NL::Pixel::ColorCode::Black);
}

/**
 * @brief Create a new instance of {@link NL::Pixel}.
 * @param colorCode 24 bit color code
 */
NL::Pixel::Pixel(const uint32_t colorCode)
{
	this->setColor(colorCode);
}

/**
 * @brief Create a new instance of {@link NL::Pixel}.
 * @param colorCode one of the html color codes
 */
NL::Pixel::Pixel(const NL::Pixel::ColorCode colorCode)
{
	this->setColor(colorCode);
}

/**
 * @brief Create a new instance of {@link NL::Pixel}.
 * @param red red value
 * @param green green value
 * @param blue blue value
 */
NL::Pixel::Pixel(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	this->setColor(red, green, blue);
}

/***
 * Destroy the {@link NL::Pixel} isntance.
 */
NL::Pixel::~Pixel()
{
}

/**
 * @brief Set the pixel color.
 * @param colorCode 24 bit color code
 */
void NL::Pixel::setColor(const uint32_t colorCode)
{
	this->red = (colorCode & 0xff0000) >> 16;
	this->green = (colorCode & 0x00ff00) >> 8;
	this->blue = (colorCode & 0x0000ff);
}

/**
 * @brief Set the pixel color.
 * @param colorCode one of the html color codes
 */
void NL::Pixel::setColor(const NL::Pixel::ColorCode colorCode)
{
	this->red = (static_cast<uint32_t>(colorCode) & 0xff0000) >> 16;
	this->green = (static_cast<uint32_t>(colorCode) & 0x00ff00) >> 8;
	this->blue = (static_cast<uint32_t>(colorCode) & 0x0000ff);
}

/**
 * @brief Set the pixel color.
 * @param red red value
 * @param green green value
 * @param blue blue value
 */
void NL::Pixel::setColor(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	this->red = red;
	this->green = green;
	this->blue = blue;
}
