/**
 * @file GradientAnimatorMotion.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::GradientAnimatorMotion}.
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
#include "led/animator/GradientAnimatorMotion.h"

/**
 * @brief Create a new instance of {@link TL::GradientAnimatorMotion}.
 * @param gradientMode mode of the gradient
 * @param color1 first color value
 * @param color2 second color value
 */
TL::GradientAnimatorMotion::GradientAnimatorMotion(const TL::GradientAnimatorMotion::GradientMode gradientMode, const TL::Pixel color1, const TL::Pixel color2)
{
	this->gradientMode = gradientMode;
	this->color[0] = color1;
	this->color[1] = color2;
}

/**
 * @brief Destroy the {@link TL::GradientAnimatorMotion}.
 */
TL::GradientAnimatorMotion::~GradientAnimatorMotion()
{
}

/**
 * @brief Initialize the {@link TL::GradientAnimatorMotion}.
 * @param ledStrip LED strip with the pixel data
 */
void TL::GradientAnimatorMotion::init(TL::LedStrip &ledStrip)
{
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(TL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render the gradient to the vector holding the LED pixel data
 * @param ledStrip LED strip with the pixel data
 */
void TL::GradientAnimatorMotion::render(TL::LedStrip &ledStrip)
{
	if (ledStrip.getLedCount() == 2)
	{
		const float motionOffset = (this->getMotionOffset() - 0.5f) * 2.0f;
		if (motionOffset < 0.0f)
		{
			ledStrip.setPixel(TL::Pixel(this->color[0].red, this->color[0].green, this->color[0].blue), 0);
			ledStrip.setPixel(
				TL::Pixel(
					(-motionOffset * this->color[0].red + (1 + motionOffset) * this->color[1].red),
					(-motionOffset * this->color[0].green + (1 + motionOffset) * this->color[1].green),
					(-motionOffset * this->color[0].blue + (1 + motionOffset) * this->color[1].blue)),
				1);
		}
		else
		{
			ledStrip.setPixel(
				TL::Pixel(
					(motionOffset * this->color[1].red + (1 - motionOffset) * this->color[0].red),
					(motionOffset * this->color[1].green + (1 - motionOffset) * this->color[0].green),
					(motionOffset * this->color[1].blue + (1 - motionOffset) * this->color[0].blue)),
				0);
			ledStrip.setPixel(TL::Pixel(this->color[1].red, this->color[1].green, this->color[1].blue), 1);
		}
	}
	else
	{
		float motionOffset = (ledStrip.getLedCount() - 1) * this->getMotionOffset();
		if (motionOffset < 0.01f)
		{
			motionOffset = 0.01f;
		}
		else if (motionOffset > ledStrip.getLedCount() - 1.01f)
		{
			motionOffset = ledStrip.getLedCount() - 1.01f;
		}

		for (uint16_t i = 0; i < ledStrip.getLedCount(); i++)
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
					position = 0.5f + ((float)i - motionOffset) / ((ledStrip.getLedCount() - 1) - motionOffset) * 0.5f;
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
					position = 1.0f - ((float)i - motionOffset) / ((ledStrip.getLedCount() - 1) - motionOffset);
				}
			}
			ledStrip.setPixel(
				TL::Pixel(
					(position * this->color[0].red + (1 - position) * this->color[1].red),
					(position * this->color[0].green + (1 - position) * this->color[1].green),
					(position * this->color[0].blue + (1 - position) * this->color[1].blue)),
				i);
		}
	}

	if (this->reverse)
	{
		this->reversePixels(ledStrip);
	}
	this->applyBrightness(ledStrip);
}

/**
 * @brief Get the motion based offset value between 0.0 and 1.0.
 * @return motion based offset value
 */
float TL::GradientAnimatorMotion::getMotionOffset()
{
	float motionValue = 0.0f;
	if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_ACC_X_G)
	{
		motionValue = this->motionSensorData.accXG;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_ACC_Y_G)
	{
		motionValue = this->motionSensorData.accYG;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_ACC_Z_G)
	{
		motionValue = this->motionSensorData.accZG;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_GY_X_DEG)
	{
		motionValue = this->motionSensorData.gyroXDeg / 30.0f;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_GY_Y_DEG)
	{
		motionValue = this->motionSensorData.gyroYDeg / 30.0f;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_GY_Z_DEG)
	{
		motionValue = this->motionSensorData.gyroZDeg / 30.0f;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_PITCH)
	{
		motionValue = this->motionSensorData.pitch / 30.0f;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_ROLL)
	{
		motionValue = this->motionSensorData.roll / 30.0f;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_ROLL_COMPENSATED_ACC_X_G)
	{
		motionValue = this->motionSensorData.rollCompensatedAccXG;
	}
	else if (this->getDataSource() == TL::LedAnimator::DataSource::DS_MOTION_PITCH_COMPENSATED_ACC_Y_G)
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

	return 0.5f + motionValue;
}