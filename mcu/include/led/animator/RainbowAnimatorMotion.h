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

#include <vector>
#include "led/animator/LedAnimator.h"
#include "sensor/MotionSensor.h"

namespace TL
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
		RainbowAnimatorMotion(const TL::RainbowAnimatorMotion::RainbowMode rainbowMode, const TL::MotionSensor::MotionSensorValue motionSensorValue);
		~RainbowAnimatorMotion();

		void init(std::vector<CRGB> &pixels);
		void render(std::vector<CRGB> &pixels);

		void setRainbowMode(const TL::RainbowAnimatorMotion::RainbowMode rainbowMode);
		void setMotionSensorValue(const TL::MotionSensor::MotionSensorValue motionSensorValue);

	private:
		float angle;
		TL::RainbowAnimatorMotion::RainbowMode rainbowMode;
		TL::MotionSensor::MotionSensorValue motionSensorValue;

		float getMotionSpeed();
	};
}

#endif