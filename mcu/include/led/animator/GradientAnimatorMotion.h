/**
 * @file GradientAnimatorMotion.h
 * @author TheRealKasumi
 * @brief Contains a class to render a gradient based on the cars motion.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef GRADIENT_ANIMATOR_MOTION_H
#define GRADIENT_ANIMATOR_MOTION_H

#include <math.h>
#include "led/animator/LedAnimator.h"
#include "sensor/MotionSensor.h"

namespace TesLight
{
	class GradientAnimatorMotion : public LedAnimator
	{
	public:
		enum GradientMode
		{
			GRADIENT_LINEAR,
			GRADIENT_CENTER
		};

		GradientAnimatorMotion();
		GradientAnimatorMotion(const TesLight::GradientAnimatorMotion::GradientMode gradientMode, TesLight::MotionSensor::MotionSensorValue motionSensorValue, const CRGB color1, const CRGB color2);
		~GradientAnimatorMotion();

		void init();
		void render();

		void setGradientMode(const TesLight::GradientAnimatorMotion::GradientMode gradientMode);
		void setColor(const CRGB color1, const CRGB color2);
		void setMotionSensorValue(const TesLight::MotionSensor::MotionSensorValue motionSensorValue);

	private:
		TesLight::GradientAnimatorMotion::GradientMode gradientMode;
		CRGB color[2];
		TesLight::MotionSensor::MotionSensorValue motionSensorValue;
	};
}

#endif