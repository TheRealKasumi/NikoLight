/**
 * @file LedAnimator.cpp
 * @author TheRealKasumi
 * @brief Contains the implementation of {@link TL::LedAnimator}.
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
#include "led/animator/LedAnimator.h"

/**
 * @brief Create a new instance of {@link TL::LedAnimator}.
 */
TL::LedAnimator::LedAnimator()
{
	this->dataSource = TL::LedAnimator::DataSource::DS_NONE;
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
 * @brief Set the data source used by the animation.
 * @param dataSource data source the animation is using
 */
void TL::LedAnimator::setDataSource(const TL::LedAnimator::DataSource dataSource)
{
	this->dataSource = dataSource;
}

/**
 * @brief Get the currently used data source used by the animation.
 * @return currently used data source
 */
TL::LedAnimator::DataSource TL::LedAnimator::getDataSource()
{
	return this->dataSource;
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
 * @param motionSensorData motion sensor data
 */
void TL::LedAnimator::setMotionSensorData(const TL::MotionSensor::MotionSensorData &motionSensorData)
{
	this->motionSensorData = motionSensorData;
}

/**
 * @brief Get the currently set motion sensor data.
 * @return currently set motion sensor data
 */
TL::MotionSensor::MotionSensorData &TL::LedAnimator::getMotionSensorData()
{
	return this->motionSensorData;
}

/**
 * @brief Set the audio analysis data to be used by an animator.
 * @param audioAnalysis audio analysis
 */
void TL::LedAnimator::setAudioAnalysis(const TL::AudioUnit::AudioAnalysis &audioAnalysis)
{
	this->audioAnalysis = audioAnalysis;
}

/**
 * @brief Get the audio analysis data.
 * @return audio analysis data
 */
TL::AudioUnit::AudioAnalysis &TL::LedAnimator::getAudioAnalysis()
{
	return this->audioAnalysis;
}

/**
 * @brief Reverse the order of all pixels to reverse the animation.
 * @param ledStrip LED strip with the pixel data
 */
void TL::LedAnimator::reversePixels(TL::LedStrip &ledStrip)
{
	for (size_t i = 0; i < ledStrip.getLedCount() / 2; i++)
	{
		const TL::Pixel copy = ledStrip.getPixel(i);
		const size_t inverseIndex = (ledStrip.getLedCount() - 1) - i;
		ledStrip.setPixel(ledStrip.getPixel(inverseIndex), i);
		ledStrip.setPixel(copy, inverseIndex);
	}
}

/**
 * @brief Apply the brightness settings to all pixels.
 * @param ledStrip LED strip with the pixel data
 */
void TL::LedAnimator::applyBrightness(TL::LedStrip &ledStrip)
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
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		TL::Pixel pixel = ledStrip.getPixel(i);
		pixel.red *= totalBrightness;
		pixel.green *= totalBrightness;
		pixel.blue *= totalBrightness;
		ledStrip.setPixel(pixel, i);
	}
}

/**
 * @brief Calculate a random value between a minimum and maximum.
 * @param min minimum value
 * @param max maximum value
 * @return random value within given bounds
 */
int32_t TL::LedAnimator::random(const int32_t min, const int32_t max)
{
	return rand() % (max - min) + min;
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
	factor -= static_cast<int>(factor);
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
	factor -= static_cast<int>(factor);
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