/**
 * @file LedAnimator.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TL::LedAnimator}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/LedAnimator.h"

/**
 * @brief Create a new instance of {@link TL::LedAnimator}.
 */
TL::LedAnimator::LedAnimator()
{
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
 * @brief Destroy the {@link TL::LedAnimator} instance.
 */
TL::LedAnimator::~LedAnimator()
{
}

/**
 * @brief Set the animation speed.
 * @param speed animation speed
 */
void TL::LedAnimator::setSpeed(const uint8_t speed)
{
	this->speed = speed;
}

/**
 * @brief Get the animation speed.
 * @return uint8_t animation speed
 */
uint8_t TL::LedAnimator::getSpeed()
{
	return this->speed;
}

/**
 * @brief Set the animation offset.
 * @param offset animation offset
 */
void TL::LedAnimator::setOffset(const uint16_t offset)
{
	this->offset = offset;
}

/**
 * @brief Get the animation offset.
 * @return animation offset
 */
uint16_t TL::LedAnimator::getOffset()
{
	return this->offset;
}

/**
 * @brief Set the animation brightness.
 * @param animationBrightness brightness of the animation from 0.0 to 1.0
 */
void TL::LedAnimator::setAnimationBrightness(const float animationBrightness)
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
 * @return animation brightness from 0.0 to 1.0
 */
float TL::LedAnimator::getAnimationBrightness()
{
	return this->animationBrightness;
}

/**
 * @brief Set the ambient brightness.
 * @param ambientBrightness ambient brightness from 0.0 to 1.0
 */
void TL::LedAnimator::setAmbientBrightness(const float ambientBrightness)
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
 * @return ambient brightness from 0.0 to 1.0
 */
float TL::LedAnimator::getAmbientBrightness()
{
	return this->ambientBrightness;
}

/**
 * @brief Set the fading speed.
 * @param fadeSpeed fading speed from 0.0 to 1.0
 */
void TL::LedAnimator::setFadeSpeed(const float fadeSpeed)
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
 * @return fading speed from 0.0 to 1.0
 */
float TL::LedAnimator::getFadeSpeed()
{
	return this->fadeSpeed;
}

/**
 * @brief Reverse the animation.
 * @param reverse {@code true} to reverse, {@code false} to not reverse
 */
void TL::LedAnimator::setReverse(const bool reverse)
{
	this->reverse = reverse;
}

/**
 * @brief Get if the animation is reversed.
 * @return true reversed
 * @return false not reversed
 */
bool TL::LedAnimator::getReverse()
{
	return this->reverse;
}

/**
 * @brief Set the motion sensor data.
 * @param motionSensorData instance of {@link TL::MotionSensor::MotionSensorData}
 */
void TL::LedAnimator::setMotionSensorData(const TL::MotionSensor::MotionSensorData &motionSensorData)
{
	this->motionSensorData = motionSensorData;
}

/**
 * @brief Get the currently set and used motion sensor data.
 * @return currently set and used motion sensor data
 */
TL::MotionSensor::MotionSensorData TL::LedAnimator::getMotionSensorData()
{
	return this->motionSensorData;
}

/**
 * @brief Apply the brightness settings to all pixels.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::LedAnimator::applyBrightness(std::vector<CRGB> &pixels)
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
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i).setRGB(pixels.at(i).r * totalBrightness, pixels.at(i).g * totalBrightness, pixels.at(i).b * totalBrightness);
	}
}

/**
 * @brief Create a trapezoid waveform. The returned value depends on the input angle.
 * @param angle input angle in degree
 * @return float value between 0.0 and 1.0 representing the trapezoid
 */
float TL::LedAnimator::trapezoid(float angle)
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

/**
 * @brief Create a trapezoid waveform. The returned value depends on the input angle.
 * 		  Compared to the first version it has smoother edges.
 * @param angle input angle in degree
 * @return float value between 0.0 and 1.0 representing the trapezoid
 */
float TL::LedAnimator::trapezoid2(float angle)
{
	// This will limit the angle to [0...360]
	float factor = angle / 360.0f;
	factor -= (int)factor;
	angle = factor * 360.0f;
	if (angle < 0.0f)
	{
		angle = 360.0f + angle;
	}

	if ((angle >= 0.0f && angle < 40.0f) || (angle >= 320.0f && angle <= 360.0f))
	{
		return 1.0f;
	}

	else if (angle >= 40.0f && angle < 140.0f)
	{
		return 1.0f - (angle - 40.0f) / 100.0f;
	}

	else if (angle >= 140.0f && angle < 220.0f)
	{
		return 0.0f;
	}

	else if (angle >= 220.0f && angle < 320)
	{
		return (angle - 220.0f) / 100.0f;
	}

	return 0.0f;
}