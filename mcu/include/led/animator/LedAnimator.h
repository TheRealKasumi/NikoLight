/**
 * @file LedAnimator.h
 * @author TheRealKasumi
 * @brief Contains a base class for all led animators.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LED_ANIMATOR_H
#define LED_ANIMATOR_H

#include <stdint.h>
#include <math.h>
#include <vector>

#include "FastLED.h"
#include "sensor/MotionSensor.h"

namespace TL
{
	class LedAnimator
	{
	public:
		LedAnimator();
		virtual ~LedAnimator() = 0;

		void setSpeed(const uint8_t speed);
		uint8_t getSpeed();

		void setOffset(const uint16_t offset);
		uint16_t getOffset();

		void setAnimationBrightness(const float animationBrightness);
		float getAnimationBrightness();

		void setAmbientBrightness(const float ambientBrightness);
		float getAmbientBrightness();

		void setFadeSpeed(const float fadeSpeed);
		float getFadeSpeed();

		void setReverse(const bool reverse);
		bool getReverse();

		void setMotionSensorData(const TL::MotionSensor::MotionSensorData &motionSensorData);
		TL::MotionSensor::MotionSensorData getMotionSensorData();

		virtual void init(std::vector<CRGB> &pixels) = 0;
		virtual void render(std::vector<CRGB> &pixels) = 0;

	protected:
		uint8_t speed;
		uint16_t offset;
		float animationBrightness;
		float ambientBrightness;
		float smoothedAmbBrightness;
		float fadeSpeed;
		bool reverse;

		TL::MotionSensor::MotionSensorData motionSensorData;

		void applyBrightness(std::vector<CRGB> &pixels);
		static float trapezoid(float angle);
		static float trapezoid2(float angle);
	};
}

#endif