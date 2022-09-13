/**
 * @file Pixel.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::Pixel}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/driver/Pixel.h"

/**
 * @brief Create a new instance of {@link TesLight::Pixel}.
 */
TesLight::Pixel::Pixel()
{
	this->color = 0;
}

/**
 * @brief Destroy the instance of {@link TesLight::Pixel}.
 */
TesLight::Pixel::~Pixel()
{
}

/**
 * @brief Set the 24 bit color value directly.
 * @param color 24 bit color value
 */
void TesLight::Pixel::setColor(const uint32_t color)
{
	this->color = color;
}

/**
 * @brief Set the red channel.
 * @param red value
 */
void TesLight::Pixel::setRed(const uint8_t red)
{
	this->color &= 0x00ff00ff;
	this->color |= red << 8;
}

/**
 * @brief Set the green channel.
 * @param green value
 */
void TesLight::Pixel::setGreen(const uint8_t green)
{
	this->color &= 0x0000ffff;
	this->color |= green << 16;
}

/**
 * @brief Set the blue channel.
 * @param blue value
 */
void TesLight::Pixel::setBlue(uint8_t blue)
{
	this->color &= 0x00ffff00;
	this->color |= blue;
}

/**
 * @brief Get the 24 bit color value.
 * @return uint32_t 24 bit color value
 */
uint32_t TesLight::Pixel::getColor()
{
	return this->color;
}

/**
 * @brief Get the red channel.
 * @return uint8_t red value
 */
uint8_t TesLight::Pixel::getRed()
{
	return (this->color & 0x0000ff00) >> 8;
}

/**
 * @brief Get the green channel.
 * @return uint8_t green value
 */
uint8_t TesLight::Pixel::getGreen()
{
	return (this->color & 0x00ff0000) >> 16;
}

/**
 * @brief Get the blue channel.
 * @return uint8_t blue value
 */
uint8_t TesLight::Pixel::getBlue()
{
	return this->color & 0x000000ff;
}
