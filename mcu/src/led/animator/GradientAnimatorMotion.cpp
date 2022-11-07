/**
 * @file GradientAnimatorMotion.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::GradientAnimatorMotion}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/GradientAnimatorMotion.h"

/**
 * @brief Create a new instance of {@link TesLight::GradientAnimatorMotion}.
 */
TesLight::GradientAnimatorMotion::GradientAnimatorMotion()
{
	this->gradientMode = TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR;
	this->color[0] = CRGB::Black;
	this->color[2] = CRGB::Black;
	this->motionSensorValue = TesLight::MotionSensor::MotionSensorValue::ACC_X_G;
}

/**
 * @brief Create a new instance of {@link TesLight::GradientAnimatorMotion}.
 * @param gradientMode mode of the gradient
 * @param motionSensorValue value that is influencing the animation
 * @param color1 first color value
 * @param color2 second color value
 */
TesLight::GradientAnimatorMotion::GradientAnimatorMotion(const TesLight::GradientAnimatorMotion::GradientMode gradientMode, TesLight::MotionSensor::MotionSensorValue motionSensorValue, const CRGB color1, const CRGB color2)
{
	this->gradientMode = gradientMode;
	this->color[0] = color1;
	this->color[1] = color2;
	this->motionSensorValue = motionSensorValue;
}

/**
 * @brief Destroy the {@link TesLight::GradientAnimatorMotion}.
 */
TesLight::GradientAnimatorMotion::~GradientAnimatorMotion()
{
}

/**
 * @brief Initialize the {@link TesLight::GradientAnimatorMotion}.
 */
void TesLight::GradientAnimatorMotion::init()
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i] = CRGB::Black;
	}
}

/**
 * @brief Render the gradient to the {@link TesLight::Pixel} array.
 */
void TesLight::GradientAnimatorMotion::render()
{
	float motionValue = this->speed / 128.0f;
	if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::ACC_X_G)
	{
		motionValue *= this->motionSensorData.accXG;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::ACC_Y_G)
	{
		motionValue *= this->motionSensorData.accYG;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::ACC_Z_G)
	{
		motionValue *= this->motionSensorData.accZG;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::GY_X_DEG)
	{
		motionValue *= this->motionSensorData.gyroXDeg / 30.0f;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::GY_Y_DEG)
	{
		motionValue *= this->motionSensorData.gyroYDeg / 30.0f;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::GY_Z_DEG)
	{
		motionValue *= this->motionSensorData.gyroZDeg / 30.0f;
	}

	motionValue = 0.5f + motionValue / (this->reverse ? -2.0f : 2.0f);
	if (motionValue < 0.0f)
	{
		motionValue = 0.0f;
	}
	if (motionValue > 1.0f)
	{
		motionValue = 1.0f;
	}

	const uint16_t middle = this->pixelCount * motionValue;
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		float position = 0.0f;

		if (this->gradientMode == TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR)
		{
			if (i < middle)
			{
				position = i * 0.5f / middle;
			}
			else
			{
				position = 0.5f + ((float)i - middle) / (this->pixelCount - middle) * 0.5f;
			}
		}

		else if (this->gradientMode == TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER)
		{
			if (i < middle)
			{
				position = (float)i / middle;
			}
			else
			{
				position = 1.0f - ((float)i - middle) / (this->pixelCount - middle);
			}
		}

		this->pixels[i].setRGB(
			(position * this->color[this->reverse ? 0 : 1].r + (1 - position) * this->color[this->reverse ? 1 : 0].r),
			(position * this->color[this->reverse ? 0 : 1].g + (1 - position) * this->color[this->reverse ? 1 : 0].g),
			(position * this->color[this->reverse ? 0 : 1].b + (1 - position) * this->color[this->reverse ? 1 : 0].b));
	}

	this->applyBrightness();
}

/**
 * @brief Set the gradient mode.
 * @param gradientMode mode of the gradient
 */
void TesLight::GradientAnimatorMotion::setGradientMode(const TesLight::GradientAnimatorMotion::GradientMode gradientMode)
{
	this->gradientMode = gradientMode;
}

/**
 * @brief Set the color for the gradient.
 * @param color1 first color value
 * @param color2 second color value
 */
void TesLight::GradientAnimatorMotion::setColor(const CRGB color1, const CRGB color2)
{
	this->color[0] = color1;
	this->color[1] = color2;
}

/**
 * @brief Set the motion sensor value that is influencing the animation.
 * @param motionSensorValue value that is influencing the animation
 */
void TesLight::GradientAnimatorMotion::setMotionSensorValue(const TesLight::MotionSensor::MotionSensorValue motionSensorValue)
{
	this->motionSensorValue = motionSensorValue;
}