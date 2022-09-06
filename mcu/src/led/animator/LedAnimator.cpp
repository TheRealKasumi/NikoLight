/**
 * @file LedAnimator.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TesLight::LedAnimator}.
 * @version 0.0.1
 * @date 2022-09-06
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
 * @brief Create a trapezoid waveform. The returned value depends on the input angle.
 * @param angle input angle in degree
 * @return float value between 0.0 and 1.0 representing the trapezoid
 */
float TesLight::LedAnimator::trapezoid(float angle)
{
	// This will limit the angle to [0...360]
	float factor = angle / 360.0f;
	factor -= (int)factor;
	angle = factor * 360.0f;
	if (angle < 0.0f)
	{
		angle = 360.0f + angle;
	}

	if ((angle >= 0.0f && angle < 60.0f) || (angle >= 300.0f && angle <= 360.0f))
	{
		return 1.0f;
	}

	else if (angle >= 60.0f && angle < 120.0f)
	{
		return 1.0f - (angle - 60.0f) / 60.0f;
	}

	else if (angle >= 120.0f && angle < 240.0f)
	{
		return 0.0f;
	}

	else if (angle >= 240.0f && angle < 300)
	{
		return (angle - 240.0f) / 60.0f;
	}

	return 0.0f;
}