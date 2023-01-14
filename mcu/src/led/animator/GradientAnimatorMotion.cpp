/**
 * @file GradientAnimatorMotion.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::GradientAnimatorMotion}.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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
#include "led/animator/GradientAnimatorMotion.h"

/**
 * @brief Create a new instance of {@link TL::GradientAnimatorMotion}.
 * @param gradientMode mode of the gradient
 * @param motionSensorValue value that is influencing the animation
 * @param color1 first color value
 * @param color2 second color value
 */
TL::GradientAnimatorMotion::GradientAnimatorMotion(const TL::GradientAnimatorMotion::GradientMode gradientMode, TL::MotionSensor::MotionSensorValue motionSensorValue,
												   const CRGB color1, const CRGB color2)
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
	std::fill(pixels.begin(), pixels.end(), CRGB::Black);
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
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::PITCH)
	{
		motionValue = this->motionSensorData.pitch / 30.0f;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::ROLL)
	{
		motionValue = this->motionSensorData.roll / 30.0f;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::ROLL_COMPENSATED_ACC_X_G)
	{
		motionValue = this->motionSensorData.rollCompensatedAccXG;
	}
	else if (this->motionSensorValue == TL::MotionSensor::MotionSensorValue::PITCH_COMPENSATED_ACC_Y_G)
	{
		motionValue = this->motionSensorData.pitchCompensatedAccYG;
	}
	else
	{
		motionValue = 0.0f;
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