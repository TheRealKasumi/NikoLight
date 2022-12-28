/**
 * @file LedAnimator.h
 * @author TheRealKasumi
 * @brief Contains a base class for all led animators.
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