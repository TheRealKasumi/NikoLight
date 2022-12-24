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

#include <vector>
#include "led/animator/LedAnimator.h"
#include "sensor/MotionSensor.h"

namespace TL
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
		GradientAnimatorMotion(const TL::GradientAnimatorMotion::GradientMode gradientMode, TL::MotionSensor::MotionSensorValue motionSensorValue, const CRGB color1, const CRGB color2);
		~GradientAnimatorMotion();

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

		void setGradientMode(const TL::GradientAnimatorMotion::GradientMode gradientMode);
		void setColor(const CRGB color1, const CRGB color2);
		void setMotionSensorValue(const TL::MotionSensor::MotionSensorValue motionSensorValue);

	private:
		TL::GradientAnimatorMotion::GradientMode gradientMode;
		CRGB color[2];
		TL::MotionSensor::MotionSensorValue motionSensorValue;

		float getMotionOffset();
	};
}

#endif