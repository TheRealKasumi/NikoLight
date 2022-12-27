/**
 * @file LedManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::LedManager}.
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

#include "led/LedManager.h"

/**
 * @brief Create a new instance of {@link TL::LedManager}.
 * @param config pointer to the configuration
 */
TL::LedManager::LedManager(TL::Configuration *config)
{
	this->config = config;
	this->renderInterval = RENDER_INTERVAL;
	this->frameInterval = FRAME_INTERVAL;
	this->regulatorTemperature = 0.0f;
}

/**
 * @brief Destroy the {@link TL::LedManager} instance and clear memory.
 */
TL::LedManager::~LedManager()
{
	this->clearAnimations();
}

/**
 * @brief Clear and create new LED data and animators from the configuration.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::LedManager::reloadAnimations()
{
	this->clearAnimations();

	if (!this->createLedData())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create new LED data."));
		return false;
	}

	if (!this->createAnimators())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create new animators."));
		return false;
	}

	return true;
}

/**
 * @brief Clear the LED data and animators to free memory.
 */
void TL::LedManager::clearAnimations()
{
	FastLED.clear();
	this->ledData.clear();
	this->ledAnimator.clear();
	this->fseqLoader.reset();
}

/**
 * @brief Set the current ambient brightness for all underlying LED animators.
 * @param ambientBrightness ambient brightness from 0.0 to 1.0
 */
void TL::LedManager::setAmbientBrightness(const float ambientBrightness)
{
	for (size_t i = 0; i < this->ledAnimator.size(); i++)
	{
		this->ledAnimator.at(i)->setAmbientBrightness(ambientBrightness);
	}
}

/**
 * @brief Set the interval for rendering the pixels in µs.
 * The minimum frame time is currently limited to 10000µs.
 * @param renderInterval target frame time in µs
 */
void TL::LedManager::setRenderInterval(const uint32_t renderInterval)
{
	this->renderInterval = renderInterval >= 10000 ? renderInterval : 10000;
}

/**
 * @brief Get the interval for rendering the pixels.
 * @return interval in microseconds
 */
uint32_t TL::LedManager::getRenderInterval()
{
	return this->renderInterval;
}

/**
 * @brief Set the interval for outputting to the LEDs in µs.
 * The minimum frame time is currently limited to 10000µs.
 * @param frameInterval target frame time in µs
 */
void TL::LedManager::setFrameInterval(const uint32_t frameInterval)
{
	this->frameInterval = frameInterval >= 10000 ? frameInterval : 10000;
}

/**
 * @brief Get the interval for outputting to the LEDs.
 * @return interval in microseconds
 */
uint32_t TL::LedManager::getFrameInterval()
{
	return this->frameInterval;
}

/**
 * @brief Set the current motion sensor data.
 * @param motionSensorData instance of the {@link TL::MotionSensor::MotionSensorData}
 */
void TL::LedManager::setMotionSensorData(const TL::MotionSensor::MotionSensorData &motionSensorData)
{
	for (size_t i = 0; i < this->ledAnimator.size(); i++)
	{
		this->ledAnimator.at(i)->setMotionSensorData(motionSensorData);
	}
}

/**
 * @brief Set the regulator temperature. Once a critical temperature is reached, the brightness will be reduced.
 * @param regulatorTemperature temperature of the generator in degree celsius.
 */
void TL::LedManager::setRegulatorTemperature(const float regulatorTemperature)
{
	this->regulatorTemperature = regulatorTemperature;
}

/**
 * @brief Get the total power draw of all LEDs that has been calculated.
 * @return total power draw in W
 */
float TL::LedManager::getLedPowerDraw()
{
	float regulatorPower[REGULATOR_COUNT];
	this->calculateRegulatorPowerDraw(regulatorPower);

	float sum = 0.0f;
	for (uint8_t i = 0; i < REGULATOR_COUNT; i++)
	{
		sum += regulatorPower[i];
	}
	return sum;
}

/**
 * @brief Get the total number of LEDs.
 * @return total number of LEDs
 */
size_t TL::LedManager::getLedCount()
{
	size_t count = 0;
	for (size_t i = 0; i < this->ledData.size(); i++)
	{
		count += this->ledData.at(i).size();
	}
	return count;
}

/**
 * @brief Render all LEDs using their animators.
 */
void TL::LedManager::render()
{
	for (size_t i = 0; i < this->ledAnimator.size() && i < this->ledData.size(); i++)
	{
		this->ledAnimator.at(i)->render(this->ledData.at(i));
	}
	this->limitPowerConsumption();
	this->limitRegulatorTemperature();
}

/**
 * @brief Show the current LED data.
 */
void TL::LedManager::show()
{
	FastLED.show();
}

/**
 * @brief Create the LED data and assign it to the FastLED library.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::LedManager::createLedData()
{
	this->ledData.assign(LED_NUM_ZONES, std::vector<CRGB>());
	for (size_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TL::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		this->ledData.at(i).assign(ledConfig.ledCount, CRGB::Black);

		switch (ledConfig.ledPin)
		{
		case 13:
			FastLED.addLeds<NEOPIXEL, 13>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 14:
			FastLED.addLeds<NEOPIXEL, 14>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 15:
			FastLED.addLeds<NEOPIXEL, 15>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 16:
			FastLED.addLeds<NEOPIXEL, 16>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 17:
			FastLED.addLeds<NEOPIXEL, 17>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 21:
			FastLED.addLeds<NEOPIXEL, 21>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 22:
			FastLED.addLeds<NEOPIXEL, 22>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 25:
			FastLED.addLeds<NEOPIXEL, 25>(&this->ledData.at(i).front(), ledConfig.ledCount);
			break;
		default:
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to link the created pixel data to the FastLED lib because the pin is invalid. It must be one of [13, 14, 15, 16, 17, 21, 22, 25]."));
			return false;
		}
	}
	return true;
}

/**
 * @brief Create the LED animators based on the configuration.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::LedManager::createAnimators()
{
	// Custom animations will be used when the first animator type is set to 255
	// The used file identifier is set by the custom fields [10-13]
	// Field 14 is reserved to store the previous, calculated animation type
	const bool customAnimation = this->config->getLedConfig(0).type == 255;
	uint32_t identifier = 0;
	memcpy(&identifier, &this->config->getLedConfig(0).customField[10], sizeof(identifier));
	if (!customAnimation)
	{
		const size_t ledCount = this->getLedCount();
		if (ledCount <= 850)
		{
			// 60 FPS
			this->setRenderInterval(RENDER_INTERVAL);
			this->setFrameInterval(FRAME_INTERVAL);
		}
		else if (ledCount > 850 && ledCount <= 1000)
		{
			// 40 FPS
			this->setRenderInterval(RENDER_INTERVAL);
			this->setFrameInterval(FRAME_INTERVAL * 1.5f);
		}
		else
		{
			// 30 FPS
			this->setRenderInterval(RENDER_INTERVAL);
			this->setFrameInterval(FRAME_INTERVAL * 2.0f);
		}

		return this->loadCalculatedAnimations();
	}
	else
	{
		String fileName;
		if (TL::FileUtil::getFileNameFromIdentifier(&SD, FSEQ_DIRECTORY, identifier, fileName) && fileName.length() > 0)
		{
			this->fseqLoader.reset(new TL::FseqLoader(&SD));
			if (!this->fseqLoader->loadFromFile(FSEQ_DIRECTORY + (String)F("/") + fileName))
			{
				TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load fseq file. The animation can not be played."));
				this->fseqLoader.reset();
				return false;
			}
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to determine file name for animation file with id ") + String(identifier) + F("."));
			return false;
		}

		this->setRenderInterval((uint32_t)this->fseqLoader->getHeader().stepTime * 1000);
		this->setFrameInterval((uint32_t)this->fseqLoader->getHeader().stepTime * 1000);
		return this->loadCustomAnimation();
	}
}

/**
 * @brief Load animators for calculated animations.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::LedManager::loadCalculatedAnimations()
{
	this->ledAnimator.resize(LED_NUM_ZONES);
	for (size_t i = 0; i < this->ledAnimator.size(); i++)
	{
		const TL::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);

		// Rainbow solid type
		if (ledConfig.type == 0)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimator(TL::RainbowAnimator::RainbowMode::RAINBOW_SOLID));
		}

		// Rainbow linear type
		else if (ledConfig.type == 1)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimator(TL::RainbowAnimator::RainbowMode::RAINBOW_LINEAR));
		}

		// Rainbow middle type
		else if (ledConfig.type == 2)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimator(TL::RainbowAnimator::RainbowMode::RAINBOW_CENTER));
		}

		// Gradient linear type
		else if (ledConfig.type == 3)
		{
			this->ledAnimator.at(i).reset(new TL::GradientAnimator(TL::GradientAnimator::GradientMode::GRADIENT_LINEAR, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Gradient center type
		else if (ledConfig.type == 4)
		{
			this->ledAnimator.at(i).reset(new TL::GradientAnimator(TL::GradientAnimator::GradientMode::GRADIENT_CENTER, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Static color type
		else if (ledConfig.type == 5)
		{
			this->ledAnimator.at(i).reset(new TL::StaticColorAnimator(CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2])));
		}

		// Color bar linear hard type
		else if (ledConfig.type == 6)
		{
			this->ledAnimator.at(i).reset(new TL::ColorBarAnimator(TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Color bar linear smooth type
		else if (ledConfig.type == 7)
		{
			this->ledAnimator.at(i).reset(new TL::ColorBarAnimator(TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_SMOOTH, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Color bar center hard type
		else if (ledConfig.type == 8)
		{
			this->ledAnimator.at(i).reset(new TL::ColorBarAnimator(TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_HARD, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Color bar center smooth type
		else if (ledConfig.type == 9)
		{
			this->ledAnimator.at(i).reset(new TL::ColorBarAnimator(TL::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_SMOOTH, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Rainbow linear motion acc x type
		else if (ledConfig.type == 10)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimatorMotion(TL::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TL::MotionSensor::MotionSensorValue::ACC_X_G));
		}

		// Rainbow linear motion acc y type
		else if (ledConfig.type == 11)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimatorMotion(TL::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TL::MotionSensor::MotionSensorValue::ACC_Y_G));
		}

		// Rainbow linear motion acc x type
		else if (ledConfig.type == 10)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimatorMotion(TL::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TL::MotionSensor::MotionSensorValue::ACC_X_G));
		}

		// Rainbow linear motion acc y type
		else if (ledConfig.type == 11)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimatorMotion(TL::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TL::MotionSensor::MotionSensorValue::ACC_Y_G));
		}

		// Rainbow center motion acc x type
		else if (ledConfig.type == 12)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimatorMotion(TL::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER, TL::MotionSensor::MotionSensorValue::ACC_X_G));
		}

		// Rainbow center motion acc y type
		else if (ledConfig.type == 13)
		{
			this->ledAnimator.at(i).reset(new TL::RainbowAnimatorMotion(TL::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER, TL::MotionSensor::MotionSensorValue::ACC_Y_G));
		}

		// Gradient linear motion acc x type
		else if (ledConfig.type == 14)
		{
			this->ledAnimator.at(i).reset(new TL::GradientAnimatorMotion(TL::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR, TL::MotionSensor::MotionSensorValue::ACC_X_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Gradient linear motion acc y type
		else if (ledConfig.type == 15)
		{
			this->ledAnimator.at(i).reset(new TL::GradientAnimatorMotion(TL::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR, TL::MotionSensor::MotionSensorValue::ACC_Y_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Gradient center motion acc x type
		else if (ledConfig.type == 16)
		{
			this->ledAnimator.at(i).reset(new TL::GradientAnimatorMotion(TL::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER, TL::MotionSensor::MotionSensorValue::ACC_X_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Gradient center motion acc y type
		else if (ledConfig.type == 17)
		{
			this->ledAnimator.at(i).reset(new TL::GradientAnimatorMotion(TL::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER, TL::MotionSensor::MotionSensorValue::ACC_Y_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5])));
		}

		// Unknown type
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is unknown. Invalid value ") + String(ledConfig.type) + F("."));
			return false;
		}

		this->ledAnimator.at(i)->setSpeed(ledConfig.speed);
		this->ledAnimator.at(i)->setOffset(ledConfig.offset);
		this->ledAnimator.at(i)->setAnimationBrightness(ledConfig.brightness / 255.0f);
		this->ledAnimator.at(i)->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		this->ledAnimator.at(i)->setReverse(ledConfig.reverse);
		this->ledAnimator.at(i)->init(this->ledData.at(i));
	}
	return true;
}

/**
 * @brief Load a custom animator and play the animation from the fseq loader.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::LedManager::loadCustomAnimation()
{
	if (this->fseqLoader->getHeader().channelCount != this->getLedCount() * 3)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file can not be used with the current LED configuration because the LED count doesn't match the channel count."));
		return false;
	}

	this->ledAnimator.resize(LED_NUM_ZONES);
	for (size_t i = 0; i < this->ledAnimator.size(); i++)
	{
		const TL::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		this->ledAnimator.at(i).reset(new TL::FseqAnimator(this->fseqLoader.get(), true));
		this->ledAnimator.at(i)->setSpeed(ledConfig.speed);
		this->ledAnimator.at(i)->setOffset(ledConfig.offset);
		this->ledAnimator.at(i)->setAnimationBrightness(ledConfig.brightness / 255.0f);
		this->ledAnimator.at(i)->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		this->ledAnimator.at(i)->setReverse(ledConfig.reverse);
		this->ledAnimator.at(i)->init(this->ledData.at(i));
	}
	return true;
}

/**
 * @brief Calculate the total power draw from each regulator using the current frame.
 * @param regulatorPower array containing the power draw per regulator after the call
 */
void TL::LedManager::calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT])
{
	for (uint8_t i = 0; i < REGULATOR_COUNT; i++)
	{
		regulatorPower[i] = 0.0f;
	}

	for (size_t i = 0; i < this->ledData.size(); i++)
	{
		TL::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		float zoneCurrent = 0.0f;
		for (size_t j = 0; j < this->ledData.at(i).size(); j++)
		{
			zoneCurrent += ledConfig.ledChannelCurrent[0] * this->ledData.at(i).at(j).r / 255.0f;
			zoneCurrent += ledConfig.ledChannelCurrent[1] * this->ledData.at(i).at(j).g / 255.0f;
			zoneCurrent += ledConfig.ledChannelCurrent[2] * this->ledData.at(i).at(j).b / 255.0f;
		}

		const uint8_t regulatorIndex = this->getRegulatorIndexFromPin(ledConfig.ledPin);
		regulatorPower[regulatorIndex] += zoneCurrent * ledConfig.ledVoltage / 1000.0f;
	}
}

/**
 * @brief Limit the power consumption of the current frame to the maxumim system power.
 */
void TL::LedManager::limitPowerConsumption()
{
	float regulatorPower[REGULATOR_COUNT];
	this->calculateRegulatorPowerDraw(regulatorPower);

	TL::Configuration::SystemConfig systemConfig = this->config->getSystemConfig();
	for (size_t i = 0; i < this->ledData.size(); i++)
	{
		TL::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		const uint8_t regulatorIndex = this->getRegulatorIndexFromPin(ledConfig.ledPin);
		float multiplicator = ((float)systemConfig.regulatorPowerLimit / REGULATOR_COUNT) / regulatorPower[regulatorIndex];
		if (multiplicator < 0.0f)
		{
			multiplicator = 0.0f;
		}
		else if (multiplicator > 1.0f)
		{
			multiplicator = 1.0f;
		}

		for (size_t j = 0; j < this->ledData.at(i).size(); j++)
		{
			this->ledData.at(i).at(j).r *= multiplicator;
			this->ledData.at(i).at(j).g *= multiplicator;
			this->ledData.at(i).at(j).b *= multiplicator;
		}
	}
}

/**
 * @brief Limit the regulator temperature by reducing the LED brightness once the high temperature is reached.
 */
void TL::LedManager::limitRegulatorTemperature()
{
	float multiplicator = 1.0f - (this->regulatorTemperature - this->config->getSystemConfig().regulatorHighTemperature) / (this->config->getSystemConfig().regulatorCutoffTemperature - this->config->getSystemConfig().regulatorHighTemperature);
	if (multiplicator < 0.0f)
	{
		multiplicator = 0.0f;
	}
	else if (multiplicator > 1.0f)
	{
		multiplicator = 1.0f;
	}

	for (size_t i = 0; i < this->ledData.size(); i++)
	{
		for (size_t j = 0; j < this->ledData.at(i).size(); j++)
		{
			this->ledData.at(i).at(j).r *= multiplicator;
			this->ledData.at(i).at(j).g *= multiplicator;
			this->ledData.at(i).at(j).b *= multiplicator;
		}
	}
}

/**
 * @brief Get the regulator index by providing the pin number.
 * @param pin physical pin number
 * @return regulator index
 */
uint8_t TL::LedManager::getRegulatorIndexFromPin(const uint8_t pin)
{
	const uint8_t regulatorMap[LED_NUM_ZONES][2] = REGULATOR_ZONE_MAPPING;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		if (regulatorMap[i][0] == pin)
		{
			return regulatorMap[i][1];
		}
	}
	return 0;
}