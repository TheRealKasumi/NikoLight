/**
 * @file RainbowAnimatorMotion.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@NL::RainbowAnimatorMotion}.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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