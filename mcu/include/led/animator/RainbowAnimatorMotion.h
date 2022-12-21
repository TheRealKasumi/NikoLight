/**
 * @file RainbowAnimatorMotion.h
 * @author TheRealKasumi
 * @brief Contains a class to render a motion based rainbow.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAINBOW_ANIMATOR_MOTION_H
#define RAINBOW_ANIMATOR_MOTION_H

#include <math.h>
#include "led/animator/LedAnimator.h"
#include "sensor/MotionSensor.h"

namespace TesLight
{
	class RainbowAnimatorMotion : public LedAnimator
	{
	public:
		enum RainbowMode
		{
			RAINBOW_SOLID,
			RAINBOW_LINEAR,
			RAINBOW_CENTER
		};

		RainbowAnimatorMotion();
		RainbowAnimatorMotion(const TesLight::RainbowAnimatorMotion::RainbowMode rainbowMode, const TesLight::MotionSensor::MotionSensorValue motionSensorValue);
		~RainbowAnimatorMotion();

		void init();
		void render();

		void setRainbowMode(const TesLight::RainbowAnimatorMotion::RainbowMode rainbowMode);
		void setMotionSensorValue(const TesLight::MotionSensor::MotionSensorValue motionSensorValue);

	private:
		float angle;
		TesLight::RainbowAnimatorMotion::RainbowMode rainbowMode;
		TesLight::MotionSensor::MotionSensorValue motionSensorValue;

		float getMotionSpeed();
	};
}

#endif