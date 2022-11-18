/**
 * @file RainbowAnimatorMotion.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@TesLight::RainbowAnimatorMotion}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/animator/RainbowAnimatorMotion.h"

/**
 * @brief Create a new instance of {@link TesLight::RainbowAnimatorMotion}.
 */
TesLight::RainbowAnimatorMotion::RainbowAnimatorMotion()
{
	this->angle = 0.0f;
	this->rainbowMode = TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_SOLID;
	this->motionSensorValue = TesLight::MotionSensor::ACC_X_G;
}

/**
 * @brief Create a new instance of {@link TesLight::RainbowAnimatorMotion}.
 *
 * @param rainbowMode display mode of the rainbow
 */
TesLight::RainbowAnimatorMotion::RainbowAnimatorMotion(const TesLight::RainbowAnimatorMotion::RainbowMode rainbowMode, const TesLight::MotionSensor::MotionSensorValue motionSensorValue)
{
	this->rainbowMode = rainbowMode;
	this->motionSensorValue = motionSensorValue;
}

/**
 * @brief Destroy the {@link TesLight::RainbowAnimatorMotion} instance.
 */
TesLight::RainbowAnimatorMotion::~RainbowAnimatorMotion()
{
}

/**
 * @brief Initialize the {@link TesLight::RainbowAnimatorMotion}.
 */
void TesLight::RainbowAnimatorMotion::init()
{
	this->angle = 0.0f;
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		this->pixels[i] = CRGB::Black;
	}
}

/**
 * @brief Render a rainbow to the {@link TesLight::Pixel} array.
 */
void TesLight::RainbowAnimatorMotion::render()
{
	const uint16_t middle = this->pixelCount / 2;
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		float redAngle = 0.0f;
		float greenAngle = 0.0f;
		float blueAngle = 0.0f;
		const float offset = this->offset / 25.0f;

		if (this->rainbowMode == TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_SOLID)
		{
			redAngle = this->angle + 0.0f;
			greenAngle = this->angle + 120.0f;
			blueAngle = this->angle + 240.0f;
		}

		else if (this->rainbowMode == TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR)
		{
			redAngle = (this->angle + 0.0f) + i * offset;
			greenAngle = (this->angle + 120.0f) + i * offset;
			blueAngle = (this->angle + 240.0f) + i * offset;
		}

		else if (this->rainbowMode == TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER)
		{
			redAngle = i < middle ? (this->angle + 0.0f) + i * offset : (this->angle + 0.0f) + (this->pixelCount - i) * offset;
			greenAngle = i < middle ? (this->angle + 120.0f) + i * offset : (this->angle + 120.0f) + (this->pixelCount - i) * offset;
			blueAngle = i < middle ? (this->angle + 240.0f) + i * offset : (this->angle + 240.0f) + (this->pixelCount - i) * offset;
		}

		this->pixels[i].setRGB(this->trapezoid(redAngle) * 255.0f, this->trapezoid(greenAngle) * 255.0f, this->trapezoid(blueAngle) * 255.0f);
	}

	this->applyBrightness();

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
 * @brief Set the mode of the rainbow animation.
 * @param rainbowMode mode of the animation
 */
void TesLight::RainbowAnimatorMotion::setRainbowMode(const TesLight::RainbowAnimatorMotion::RainbowMode rainbowMode)
{
	this->rainbowMode = rainbowMode;
}

/**
 * @brief Set the value of the motion sensor that is influencing the animation.
 * @param motionSensorValue value to influence the animation
 */
void TesLight::RainbowAnimatorMotion::setMotionSensorValue(const TesLight::MotionSensor::MotionSensorValue motionSensorValue)
{
	this->motionSensorValue = motionSensorValue;
}

/**
 * @brief Get the rainbow speed based on the motion senor value.
 * @return speed value based on the motion sensor value
 */
float TesLight::RainbowAnimatorMotion::getMotionSpeed()
{
	float speed = this->speed / 15.0f;
	if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::ACC_X_G)
	{
		speed *= this->motionSensorData.accXG;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::ACC_Y_G)
	{
		speed *= this->motionSensorData.accYG;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::ACC_Z_G)
	{
		speed *= this->motionSensorData.accZG;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::GY_X_DEG)
	{
		speed *= this->motionSensorData.gyroXDeg / 10.0f;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::GY_Y_DEG)
	{
		speed *= this->motionSensorData.gyroYDeg / 10.0f;
	}
	else if (this->motionSensorValue == TesLight::MotionSensor::MotionSensorValue::GY_Z_DEG)
	{
		speed *= this->motionSensorData.gyroZDeg / 10.0f;
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