/**
 * @file LedAnimator.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TesLight::LedAnimator}.
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
	this->animationBrightness = 0.0f;
	this->ambientBrightness = 0.0f;
	this->smoothedAmbBrightness = 0.0f;
	this->fadeSpeed = 1.0f;
	this->reverse = false;

	this->motionSensorData.accXRaw = 0;
	this->motionSensorData.accYRaw = 0;
	this->motionSensorData.accZRaw = 0;
	this->motionSensorData.gyroXRaw = 0;
	this->motionSensorData.gyroYRaw = 0;
	this->motionSensorData.gyroZRaw = 0;
	this->motionSensorData.accXG = 0;
	this->motionSensorData.accYG = 0;
	this->motionSensorData.accZG = 0;
	this->motionSensorData.gyroXDeg = 0;
	this->motionSensorData.gyroYDeg = 0;
	this->motionSensorData.gyroZDeg = 0;
}

/**
 * @brief Destroy the {@link TesLight::LedAnimator} instance.
 */
TesLight::LedAnimator::~LedAnimator()
{
}

/**
 * @brief Set the {@link CRGB} array reference for the {@link LedAnimator}.
 * @param pixels reference to the {@link CRGB} array
 */
void TesLight::LedAnimator::setPixels(CRGB *pixels)
{
	this->pixels = pixels;
}

/**
 * @brief Get the reference to the {@link CRGB} array.
 * @return Reference to the {@link CRGB} array
 */
CRGB *TesLight::LedAnimator::getPixels()
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
 * @brief Get the number of {@link CRGB} that are used by the {@link LedAnimator}.
 * @return uint16_t number of {@link CRGB}
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
 * @brief Set the animation brightness.
 * @param animationBrightness brightness of the animation from 0.0 to 1.0
 */
void TesLight::LedAnimator::setAnimationBrightness(const float animationBrightness)
{
	this->animationBrightness = animationBrightness;
	if (this->animationBrightness < 0.0f)
	{
		this->animationBrightness = 0.0f;
	}
	else if (this->animationBrightness > 1.0f)
	{
		this->animationBrightness = 1.0f;
	}
}

/**
 * @brief Get the animation brightness.
 * @return float animation brightness from 0.0 to 1.0
 */
float TesLight::LedAnimator::getAnimationBrightness()
{
	return this->animationBrightness;
}

/**
 * @brief Set the ambient brightness.
 * @param ambientBrightness ambient brightness from 0.0 to 1.0
 */
void TesLight::LedAnimator::setAmbientBrightness(const float ambientBrightness)
{
	this->ambientBrightness = ambientBrightness;
	if (this->ambientBrightness < 0.0f)
	{
		this->ambientBrightness = 0.0f;
	}
	else if (this->ambientBrightness > 1.0f)
	{
		this->ambientBrightness = 1.0f;
	}
}

/**
 * @brief Get the ambient brightness.
 * @return float ambient brightness from 0.0 to 1.0
 */
float TesLight::LedAnimator::getAmbientBrightness()
{
	return this->ambientBrightness;
}

/**
 * @brief Set the fading speed.
 * @param fadeSpeed fading speed from 0.0 to 1.0
 */
void TesLight::LedAnimator::setFadeSpeed(const float fadeSpeed)
{
	this->fadeSpeed = fadeSpeed;
	if (this->fadeSpeed < 0.0f)
	{
		this->fadeSpeed = 0.0f;
	}
	else if (this->fadeSpeed > 1.0f)
	{
		this->fadeSpeed = 1.0f;
	}
}

/**
 * @brief Get the fading speed.
 * @return float fading speed from 0.0 to 1.0
 */
float TesLight::LedAnimator::getFadeSpeed()
{
	return this->fadeSpeed;
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
 * @brief Set the motion sensor data.
 * @param motionSensorData instance of {@link TesLight::MotionSensor::MotionSensorData}
 */
void TesLight::LedAnimator::setMotionSensorData(const TesLight::MotionSensor::MotionSensorData motionSensorData)
{
	this->motionSensorData = motionSensorData;
}

/**
 * @brief Get the currently set and used motion sensor data.
 * @return TesLight::MotionSensor::MotionSensorData currently set and used motion sensor data
 */
TesLight::MotionSensor::MotionSensorData TesLight::LedAnimator::getMotionSensorData()
{
	return this->motionSensorData;
}

/**
 * @brief Apply the brightness settings to all pixels.
 */
void TesLight::LedAnimator::applyBrightness()
{
	if (this->smoothedAmbBrightness < this->ambientBrightness)
	{
		this->smoothedAmbBrightness += this->fadeSpeed;
		if (this->smoothedAmbBrightness > this->ambientBrightness)
		{
			this->smoothedAmbBrightness = this->ambientBrightness;
		}
	}
	else if (this->smoothedAmbBrightness > this->ambientBrightness)
	{
		this->smoothedAmbBrightness -= this->fadeSpeed;
		if (this->smoothedAmbBrightness < this->ambientBrightness)
		{
			this->smoothedAmbBrightness = this->ambientBrightness;
		}
	}

	const float totalBrightness = this->animationBrightness * this->smoothedAmbBrightness;
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i].setRGB(this->pixels[i].r * totalBrightness, this->pixels[i].g * totalBrightness, this->pixels[i].b * totalBrightness);
	}
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