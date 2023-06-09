/**
 * @file LedAnimator.h
 * @author TheRealKasumi
 * @brief Contains a base class for all led animators.
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
#ifndef LED_ANIMATOR_H
#define LED_ANIMATOR_H

#include <stdint.h>
#include <math.h>
#include <vector>

#include "led/driver/LedStrip.h"
#include "sensor/MotionSensor.h"
#include "hardware/AudioUnit.h"

namespace NL
{
	class LedAnimator
	{
	public:
		enum class DataSource : uint8_t
		{
			DS_NONE = 0,							  // No data source, animator is providing the data itself
			DS_RANDOM = 1,							  // Random data is used for the animation
			DS_MOTION_ACC_X_RAW = 2,				  // Raw x acceleration is used for the animation
			DS_MOTION_ACC_Y_RAW = 3,				  // Raw y acceleration is used for the animation
			DS_MOTION_ACC_Z_RAW = 4,				  // Raw z acceleration is used for the animation
			DS_MOTION_GY_X_RAW = 5,					  // Raw x rotation is used for the animation
			DS_MOTION_GY_Y_RAW = 6,					  // Raw y rotation is used for the animation
			DS_MOTION_GY_Z_RAW = 7,					  // Raw z rotation is used for the animation
			DS_MOTION_ACC_X_G = 8,					  // Acceleration on x axis in g is used for the animation
			DS_MOTION_ACC_Y_G = 9,					  // Acceleration on y axis in g is used for the animation
			DS_MOTION_ACC_Z_G = 10,					  // Acceleration on z axis in g is used for the animation
			DS_MOTION_GY_X_DEG = 11,				  // Rotation around the x axis in deg/s is used for the animation
			DS_MOTION_GY_Y_DEG = 12,				  // Rotation around the y axis in deg/s is used for the animation
			DS_MOTION_GY_Z_DEG = 13,				  // Rotation around the z axis in deg/s is used for the animation
			DS_MOTION_PITCH = 14,					  // Pitch angle of the unit is used for the animation
			DS_MOTION_ROLL = 15,					  // Roll angle of the unit is used for the animation
			DS_MOTION_YAW = 16,						  // Yaw angle of the unit is used for the animation
			DS_MOTION_ROLL_COMPENSATED_ACC_X_G = 17,  // Roll angle compensated acceleration on x axis in g is used for the animation
			DS_MOTION_PITCH_COMPENSATED_ACC_Y_G = 18, // Pitch angle compensated acceleration on y axis in g is used for the animation
			DS_AUDIO_FREQUENCY_TRIGGER = 19,		  // Frequency triggers of the audio unit are used for the animation
			DS_AUDIO_FREQUENCY_VALUE = 20,			  // Frequency value of the audio unit are used for the animation
			DS_AUDIO_VOLUME_PEAK = 21				  // Volume peak of the audio unit are used for the animation
		};

		LedAnimator();
		virtual ~LedAnimator() = 0;

		void setDataSource(const NL::LedAnimator::DataSource dataSource);
		NL::LedAnimator::DataSource getDataSource();

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

		void setMotionSensorData(const NL::MotionSensor::MotionSensorData &motionSensorData);
		NL::MotionSensor::MotionSensorData &getMotionSensorData();

		void setAudioAnalysis(const NL::AudioUnit::AudioAnalysis &audioAnalysis);
		NL::AudioUnit::AudioAnalysis &getAudioAnalysis();

		virtual void init(NL::LedStrip &ledStrip) = 0;
		virtual void render(NL::LedStrip &ledStrip) = 0;

	protected:
		NL::LedAnimator::DataSource dataSource;
		uint8_t speed;
		uint16_t offset;
		float animationBrightness;
		float ambientBrightness;
		float smoothedAmbBrightness;
		float fadeSpeed;
		bool reverse;

		NL::MotionSensor::MotionSensorData motionSensorData;
		NL::AudioUnit::AudioAnalysis audioAnalysis;

		void reversePixels(NL::LedStrip &ledStrip);
		void applyBrightness(NL::LedStrip &ledStrip);
		static int32_t random(const int32_t min, const int32_t max);
		static float trapezoid(float angle);
		static float trapezoid2(float angle);
	};
}

#endif