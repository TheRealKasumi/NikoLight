/**
 * @file LedAnimator.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TesLight::LedAnimator}.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/LedAnimator.h"

/**
 * @brief Create a new instance of {@link TesLight::LedAnimator}.
 */
TesLight::LedAnimator::LedAnimator()
{
	this->pixels = nullptr;
	this->pixelCount = 0;
	this->speed = 0;
	this->offset = 0;
	this->reverse = false;
}

/**
 * @brief Destroy the {@link TesLight::LedAnimator} instance.
 */
TesLight::LedAnimator::~LedAnimator()
{
}

/**
 * @brief Set the {@link TesLight::Pixel} array reference for the {@link LedAnimator}.
 * @param pixels reference to the {@link TesLight::Pixel} array
 */
void TesLight::LedAnimator::setPixels(TesLight::Pixel *pixels)
{
	this->pixels = pixels;
}

/**
 * @brief Get the reference to the {@link TesLight::Pixel} array.
 * @return Reference to the {@link TesLight::Pixel} array
 */
TesLight::Pixel *TesLight::LedAnimator::getPixels()
{
	return this->pixels;
}

/**
 * @brief Set the number of pixels to render.
 * @param pixelCount number of pixels.
 */
void TesLight::LedAnimator::setPixelCount(const uint16_t pixelCount)
{
	this->pixelCount = pixelCount;
}

/**
 * @brief Get the number of {@link TesLight::Pixel} that are used by the {@link LedAnimator}.
 * @return uint16_t number of {@link TesLight::Pixel}
 */
uint16_t TesLight::LedAnimator::getPixelCount()
{
	return this->pixelCount;
}

/**
 * @brief Set the animation speed.
 * @param speed animation speed
 */
void TesLight::LedAnimator::setSpeed(const uint8_t speed)
{
	this->speed = speed;
}

/**
 * @brief Get the animation speed.
 * @return uint8_t animation speed
 */
uint8_t TesLight::LedAnimator::getSpeed()
{
	return this->speed;
}

/**
 * @brief Set the animation offset.
 * @param offset animation offset
 */
void TesLight::LedAnimator::setOffset(const uint16_t offset)
{
	this->offset = offset;
}

/**
 * @brief Get the animation offset.
 * @return uint16_t animation offset
 */
uint16_t TesLight::LedAnimator::getOffset()
{
	return this->offset;
}

/**
 * @brief Set the brightness of the animation. Valid range is 0.0 to 1.0.
 * @param brightness animation brightness
 */
void TesLight::LedAnimator::setBrightness(const float brightness)
{
	this->brightness = brightness;

	if (this->brightness < 0.0f)
	{
		this->brightness = 0.0f;
	}
	else if (this->brightness > 1.0f)
	{
		this->brightness = 1.0f;
	}
}

/**
 * @brief Get the brightness of the animation.
 * @return float animation brightness
 */
float TesLight::LedAnimator::getBrightness()
{
	return this->brightness;
}

/**
 * @brief Reverse the animation.
 * @param reverse {@code true} to reverse, {@code false} to not reverse
 */
void TesLight::LedAnimator::setReverse(const bool reverse)
{
	this->reverse = reverse;
}

/**
 * @brief Get if the animation is reversed.
 * @return true reversed
 * @return false not reversed
 */
bool TesLight::LedAnimator::getReverse()
{
	return this->reverse;
}

/**
 * @brief Convert deg to rad.
 * @param deg angle in deg
 * @return float angle in rad
 */
float TesLight::LedAnimator::degToRad(const float deg)
{
	return deg * 0.0174533f;
}

/**
 * @brief Return the non-negative value.
 * @param input input
 * @return float non-negative value
 */
float TesLight::LedAnimator::abs(const float input)
{
	return input >= 0.0f ? input : -input;
}

/**
 * @brief Fast cos approximation.
 * @param angle angle in rad
 * @return float cos approximation
 */
float TesLight::LedAnimator::fastCos(float angle)
{
	const float tp = 1.0f / (2.0f * 3.1415926f);
	angle *= tp;
	angle -= float(0.25f) + floor(angle + float(0.25f));
	angle *= float(16.0f) * (abs(angle) - float(0.5f));
	return angle;
}

/**
 * @brief Cut all values below 0.0.
 * @param input input
 * @return float number >= 0.0
 */
float TesLight::LedAnimator::cutNegative(const float input)
{
	if (input < 0.0f)
	{
		return 0.0f;
	}
	return input;
}