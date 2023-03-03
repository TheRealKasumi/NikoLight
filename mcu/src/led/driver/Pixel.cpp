/**
 * @file Pixel.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::Pixel} class.
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
#include "led/driver/Pixel.h"

/**
 * @brief Create a new instance of {@link TL::Pixel}.
 */
TL::Pixel::Pixel()
{
	this->setColor(TL::Pixel::ColorCode::Black);
}

/**
 * @brief Create a new instance of {@link TL::Pixel}.
 * @param colorCode 24 bit color code
 */
TL::Pixel::Pixel(const uint32_t colorCode)
{
	this->setColor(colorCode);
}

/**
 * @brief Create a new instance of {@link TL::Pixel}.
 * @param colorCode one of the html color codes
 */
TL::Pixel::Pixel(const TL::Pixel::ColorCode colorCode)
{
	this->setColor(colorCode);
}

/**
 * @brief Create a new instance of {@link TL::Pixel}.
 * @param red red value
 * @param green green value
 * @param blue blue value
 */
TL::Pixel::Pixel(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	this->setColor(red, green, blue);
}

/***
 * Destroy the {@link TL::Pixel} isntance.
 */
TL::Pixel::~Pixel()
{
}

/**
 * @brief Set the pixel color.
 * @param colorCode 24 bit color code
 */
void TL::Pixel::setColor(const uint32_t colorCode)
{
	this->red = (colorCode & 0xff0000) >> 16;
	this->green = (colorCode & 0x00ff00) >> 8;
	this->blue = (colorCode & 0x0000ff);
}

/**
 * @brief Set the pixel color.
 * @param colorCode one of the html color codes
 */
void TL::Pixel::setColor(const TL::Pixel::ColorCode colorCode)
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
void TL::Pixel::setColor(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	this->red = red;
	this->green = green;
	this->blue = blue;
}
