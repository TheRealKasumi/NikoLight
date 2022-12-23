/**
 * @file GradientAnimatorMotion.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::GradientAnimatorMotion}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/GradientAnimatorMotion.h"

/**
 * @brief Create a new instance of {@link TL::GradientAnimatorMotion}.
 */
TL::GradientAnimatorMotion::GradientAnimatorMotion()
{
	this->gradientMode = TL::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR;
	this->color[0] = CRGB::Black;
	this->color[1] = CRGB::Black;
	this->motionSensorValue = TL::MotionSensor::MotionSensorValue::ACC_X_G;
}

/**
 * @brief Create a new instance of {@link TL::GradientAnimatorMotion}.
 * @param gradientMode mode of the gradient
 * @param motionSensorValue value that is influencing the animation
 * @param color1 first color value
 * @param color2 second color value
 */
TL::GradientAnimatorMotion::GradientAnimatorMotion(const TL::GradientAnimatorMotion::GradientMode gradientMode, TL::MotionSensor::MotionSensorValue motionSensorValue, const CRGB color1, const CRGB color2)
{
	this->gradientMode = gradientMode;
	this->color[0] = color1;
	this->color[1] = color2;
	this->motionSensorValue = motionSensorValue;
}

/**
 * @brief Destroy the {@link TL::GradientAnimatorMotion}.
 */
TL::GradientAnimatorMotion::~GradientAnimatorMotion()
{
}

/**
 * @brief Initialize the {@link TL::GradientAnimatorMotion}.
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::GradientAnimatorMotion::init(std::vector<CRGB> &pixels)
{
	for (size_t i = 0; i < pixels.size(); i++)
	{
		pixels.at(i) = CRGB::Black;
	}
}

/**
 * @brief Render the gradient to the vector holding the LED pixel data
 * @param pixels reference to the vector holding the LED pixel data
 */
void TL::GradientAnimatorMotion::render(std::vector<CRGB> &pixels)
{
	if (pixels.size() == 2)
	{
		const float motionOffset = (this->getMotionOffset() - 0.5f) * 2.0f;
		if (motionOffset < 0.0f)
		{
			pixels.at(0).setRGB(this->color[0].r, this->color[0].g, this->color[0].b);
			pixels.at(1).setRGB(
				(-motionOffset * this->color[0].r + (1 + motionOffset) * this->color[1].r),
				(-motionOffset * this->color[0].g + (1 + motionOffset) * this->color[1].g),
				(-motionOffset * this->color[0].b + (1 + motionOffset) * this->color[1].b));
		}
		else
		{
			pixels.at(0).setRGB(
				(motionOffset * this->color[1].r + (1 - motionOffset) * this->color[0].r),
				(motionOffset * this->color[1].g + (1 - motionOffset) * this->color[0].g),
				(motionOffset * this->color[1].b + (1 - motionOffset) * this->color[0].b));
			pixels.at(1).setRGB(this->color[1].r, this->color[1].g, this->color[1].b);
		}
	}
	else
	{
		float motionOffset = (pixels.size() - 1) * this->getMotionOffset();
		if (motionOffset < 0.01f)
		{
			motionOffset = 0.01f;
		}
		else if (motionOffset > pixels.size() - 1.01f)
		{
			motionOffset = pixels.size() - 1.01f;
		}

		for (uint16_t i = 0; i < pixels.size(); i++)
		{
			float position = 0.0f;
			if (this->gradientMode == TL::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR)
			{
				if (i < motionOffset)
				{
					position = i * 0.5f / motionOffset;
				}
				else
				{
					position = 0.5f + ((float)i - motionOffset) / ((pixels.size() - 1) - motionOffset) * 0.5f;
				}
			}
			else if (this->gradientMode == TL::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER)
			{
				if (i < motionOffset)
				{
					position = (float)i / motionOffset;
				}
				else
				{
					position = 1.0f - ((float)i - motionOffset) / ((pixels.size() - 1) - motionOffset);
				}
			}
			pixels.at(i).setRGB(
				(position * this->color[0].r + (1 - position) * this->color[1].r),
				(position * this->color[0].g + (1 - position) * this->color[1].g),
				(position * this->color[0].b + (1 - position) * this->color[1].b));
		}
	}

	this->applyBrightness(pixels);
}

/**
 * @brief Set the gradient mode.
 * @param gradientMode mode of the gradient
 */
void TL::GradientAnimatorMotion::setGradientMode(const TL::GradientAnimatorMotion::GradientMode gradientMode)
{
	this->gradientMode = gradientMode;
}

/**
 * @brief Set the color for the gradient.
 * @param color1 first color value
 * @param color2 second color value
 */
void TL::GradientAnimatorMotion::setColor(const CRGB color1, const CRGB color2)
{
	this->color[0] = color1;
	this->color[1] = color2;
}

/**
 * @brief Set the motion sensor value that is influencing the animation.
 * @param motionSensorValue value that is influencing the animation
 */
void TL::GradientAnimatorMotion::setMotionSensorValue(const TL::MotionSensor::MotionSensorValue motionSensorValue)
{
	this->motionSensorValue = motionSensorValue;
}

/**
 * @brief Get the motion based offset value between 0.0 and 1.0.
 * @return motion based offset value
 */
float TL::GradientAnimatorMotion::getMotionOffset()
{
	float motionValue = 0.0f;
	if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::ACC_X_G)
	{
		motionValue = this->motionSensorData.accXG;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::ACC_Y_G)
	{
		motionValue = this->motionSensorData.accYG;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::ACC_Z_G)
	{
		motionValue = this->motionSensorData.accZG;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::GY_X_DEG)
	{
		motionValue = this->motionSensorData.gyroXDeg / 30.0f;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::GY_Y_DEG)
	{
		motionValue = this->motionSensorData.gyroYDeg / 30.0f;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::GY_Z_DEG)
	{
		motionValue = this->motionSensorData.gyroZDeg / 30.0f;
	}

	motionValue *= this->speed / 127.0f;
	motionValue += (this->offset / 255.0f) - 0.5f;

	if (motionValue < -0.5f)
	{
		motionValue = -0.5f;
	}
	if (motionValue > 0.5f)
	{
		motionValue = 0.5f;
	}

	return 0.5f + (this->reverse ? -motionValue : motionValue);
}