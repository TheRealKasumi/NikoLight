/**
 * @file RainbowAnimatorMotion.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@NL::RainbowAnimatorMotion}.
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
#include "led/animator/RainbowAnimatorMotion.h"

/**
 * @brief Create a new instance of {@link NL::RainbowAnimatorMotion}.
 * @param rainbowMode display mode of the rainbow
 */
NL::RainbowAnimatorMotion::RainbowAnimatorMotion(const NL::RainbowAnimatorMotion::RainbowMode rainbowMode)
{
	this->angle = 0.0f;
	this->rainbowMode = rainbowMode;
}

/**
 * @brief Destroy the {@link NL::RainbowAnimatorMotion} instance.
 */
NL::RainbowAnimatorMotion::~RainbowAnimatorMotion()
{
}

/**
 * @brief Initialize the {@link NL::RainbowAnimatorMotion}.
 * @param ledStrip LED strip with the pixel data
 */
void NL::RainbowAnimatorMotion::init(NL::LedStrip &ledStrip)
{
	this->angle = 0.0f;
	for (size_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		ledStrip.setPixel(NL::Pixel::ColorCode::Black, i);
	}
}

/**
 * @brief Render a rainbow to the vector holding the LED pixel data
 * @param ledStrip LED strip with the pixel data
 */
void NL::RainbowAnimatorMotion::render(NL::LedStrip &ledStrip)
{
	const float middle = ledStrip.getLedCount() / 2;
	for (uint16_t i = 0; i < ledStrip.getLedCount(); i++)
	{
		float redAngle = 0.0f;
		float greenAngle = 0.0f;
		float blueAngle = 0.0f;
		const float offset = this->offset / 25.0f;

		if (this->rainbowMode == NL::RainbowAnimatorMotion::RainbowMode::RAINBOW_SOLID)
		{
			redAngle = this->angle + 0.0f;
			greenAngle = this->angle + 120.0f;
			blueAngle = this->angle + 240.0f;
		}

		else if (this->rainbowMode == NL::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR)
		{
			redAngle = (this->angle + 0.0f) + i * offset;
			greenAngle = (this->angle + 120.0f) + i * offset;
			blueAngle = (this->angle + 240.0f) + i * offset;
		}

		else if (this->rainbowMode == NL::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER)
		{
			redAngle = i < middle ? (this->angle + 0.0f) + i * offset : (this->angle + 0.0f) + (ledStrip.getLedCount() - i) * offset;
			greenAngle = i < middle ? (this->angle + 120.0f) + i * offset : (this->angle + 120.0f) + (ledStrip.getLedCount() - i) * offset;
			blueAngle = i < middle ? (this->angle + 240.0f) + i * offset : (this->angle + 240.0f) + (ledStrip.getLedCount() - i) * offset;
		}

		ledStrip.setPixel(NL::Pixel(this->trapezoid(redAngle) * 255.0f, this->trapezoid(greenAngle) * 255.0f, this->trapezoid(blueAngle) * 255.0f), i);
	}

	this->applyBrightness(ledStrip);

	const float speed = this->getMotionSpeed();
	if (this->reverse)
	{
		this->angle += speed;
	}
	else
	{
		this->angle -= speed;
	}

	if (this->angle >= 360.0f)
	{
		this->angle -= 360.0f;
	}
	else if (this->angle < 0.0f)
	{
		this->angle += 360.0f;
	}
}

/**
 * @brief Get the rainbow speed based on the motion senor value.
 * @return speed value based on the motion sensor value
 */
float NL::RainbowAnimatorMotion::getMotionSpeed()
{
	float speed = this->speed / 15.0f;
	if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_ACC_X_G)
	{
		speed *= this->motionSensorData.accXG;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_ACC_Y_G)
	{
		speed *= this->motionSensorData.accYG;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_ACC_Z_G)
	{
		speed *= this->motionSensorData.accZG;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_GY_X_DEG)
	{
		speed *= this->motionSensorData.gyroXDeg / 20.0f;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_GY_Y_DEG)
	{
		speed *= this->motionSensorData.gyroYDeg / 20.0f;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_GY_Z_DEG)
	{
		speed *= this->motionSensorData.gyroZDeg / 20.0f;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_PITCH)
	{
		speed *= this->motionSensorData.pitch / 20.0f;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_ROLL)
	{
		speed *= this->motionSensorData.roll / 20.0f;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_PITCH_COMPENSATED_ACC_Y_G)
	{
		speed *= this->motionSensorData.pitchCompensatedAccYG;
	}
	else if (this->getDataSource() == NL::LedAnimator::DataSource::DS_MOTION_ROLL_COMPENSATED_ACC_X_G)
	{
		speed *= this->motionSensorData.rollCompensatedAccXG;
	}
	else
	{
		speed = 0.0f;
	}

	if (speed > 20)
	{
		speed = 20;
	}
	else if (speed < -20)
	{
		speed = -20;
	}

	return speed;
}