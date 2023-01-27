/**
 * @file LedManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::LedManager}.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
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

bool TL::LedManager::initialized = false;
std::vector<std::vector<CRGB>> TL::LedManager::ledData;
std::vector<std::unique_ptr<TL::LedAnimator>> TL::LedManager::ledAnimator;
std::unique_ptr<TL::FseqLoader> TL::LedManager::fseqLoader;
uint32_t TL::LedManager::renderInterval;
uint32_t TL::LedManager::frameInterval;
float TL::LedManager::regulatorTemperature;

/**
 * @brief Start the LED manager.
 * @return OK when the LED manager was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration was not initialized
 */
TL::LedManager::Error TL::LedManager::begin()
{
	TL::LedManager::initialized = false;
	TL::LedManager::renderInterval = RENDER_INTERVAL;
	TL::LedManager::frameInterval = FRAME_INTERVAL;
	TL::LedManager::regulatorTemperature = 0.0f;

	if (!TL::Configuration::isInitialized())
	{
		return TL::LedManager::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	TL::LedManager::initialized = true;
	return TL::LedManager::Error::OK;
}

/**
 * @brief Stop the LED manager.
 */
void TL::LedManager::end()
{
	TL::LedManager::clearAnimations();
}

/**
 * @brief Check if the LED manager is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::LedManager::isInitialized()
{
	return TL::LedManager::initialized;
}

/**
 * @brief Clear and create new LED data and animators from the configuration.
 * @return OK when the animation were reloaded
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration was not initialized
 * @return ERROR_CREATE_LED_DATA when the LED data could not be created
 * @return ERROR_UNKNOWN_ANIMATOR_TYPE when one of the animator types is unknown
 * @return ERROR_INVALID_FSEQ when a custom animation was set but the fseq file is invalid
 * @return ERROR_FILE_NOT_FOUND when the animation file was not found
 * @return ERROR_INVALID_LED_CONFIGURATION when the current LED configuration does not match the custom animation
 */
TL::LedManager::Error TL::LedManager::reloadAnimations()
{
	if (!TL::Configuration::isInitialized())
	{
		return TL::LedManager::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	TL::LedManager::clearAnimations();

	const TL::LedManager::Error ledDataError = TL::LedManager::createLedData();
	if (ledDataError != TL::LedManager::Error::OK)
	{
		return ledDataError;
	}

	const TL::LedManager::Error animatorError = TL::LedManager::createAnimators();
	if (animatorError != TL::LedManager::Error::OK)
	{
		return animatorError;
	}

	return TL::LedManager::Error::OK;
}

/**
 * @brief Clear the LED data and animators to free memory.
 */
void TL::LedManager::clearAnimations()
{
	FastLED.clear();
	TL::LedManager::ledData.clear();
	TL::LedManager::ledAnimator.clear();
	TL::LedManager::fseqLoader.reset();
}

/**
 * @brief Set the current ambient brightness for all underlying LED animators.
 * @param ambientBrightness ambient brightness from 0.0 to 1.0
 */
void TL::LedManager::setAmbientBrightness(const float ambientBrightness)
{
	for (size_t i = 0; i < TL::LedManager::ledAnimator.size(); i++)
	{
		TL::LedManager::ledAnimator.at(i)->setAmbientBrightness(ambientBrightness);
	}
}

/**
 * @brief Set the interval for rendering the pixels in µs.
 * The minimum frame time is currently limited to 10000µs.
 * @param renderInterval target frame time in µs
 */
void TL::LedManager::setRenderInterval(const uint32_t renderInterval)
{
	TL::LedManager::renderInterval = renderInterval >= 10000 ? renderInterval : 10000;
}

/**
 * @brief Get the interval for rendering the pixels.
 * @return interval in microseconds
 */
uint32_t TL::LedManager::getRenderInterval()
{
	return TL::LedManager::renderInterval;
}

/**
 * @brief Set the interval for outputting to the LEDs in µs.
 * The minimum frame time is currently limited to 10000µs.
 * @param frameInterval target frame time in µs
 */
void TL::LedManager::setFrameInterval(const uint32_t frameInterval)
{
	TL::LedManager::frameInterval = frameInterval >= 10000 ? frameInterval : 10000;
}

/**
 * @brief Get the interval for outputting to the LEDs.
 * @return interval in microseconds
 */
uint32_t TL::LedManager::getFrameInterval()
{
	return TL::LedManager::frameInterval;
}

/**
 * @brief Set the current motion sensor data.
 * @param motionSensorData motion sensor data
 */
void TL::LedManager::setMotionSensorData(const TL::MotionSensor::MotionSensorData &motionSensorData)
{
	for (size_t i = 0; i < TL::LedManager::ledAnimator.size(); i++)
	{
		TL::LedManager::ledAnimator.at(i)->setMotionSensorData(motionSensorData);
	}
}

/**
 * @brief Set the current audio analysis data.
 * @param audioAnalysis audio analysis data
 */
void TL::LedManager::setAudioAnalysis(const TL::AudioUnit::AudioAnalysis &audioAnalysis)
{
	for (size_t i = 0; i < TL::LedManager::ledAnimator.size(); i++)
	{
		TL::LedManager::ledAnimator.at(i)->setAudioAnalysis(audioAnalysis);
	}
}

/**
 * @brief Set the regulator temperature. Once a critical temperature is reached, the brightness will be reduced.
 * @param regulatorTemperature temperature of the generator in degree celsius.
 */
void TL::LedManager::setRegulatorTemperature(const float regulatorTemperature)
{
	TL::LedManager::regulatorTemperature = regulatorTemperature;
}

/**
 * @brief Get the total power draw of all LEDs that has been calculated.
 * @return total power draw in W
 */
float TL::LedManager::getLedPowerDraw()
{
	float regulatorPower[REGULATOR_COUNT];
	TL::LedManager::calculateRegulatorPowerDraw(regulatorPower);

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
	for (size_t i = 0; i < TL::LedManager::ledData.size(); i++)
	{
		count += TL::LedManager::ledData.at(i).size();
	}
	return count;
}

/**
 * @brief Render all LEDs using their animators.
 */
void TL::LedManager::render()
{
	for (size_t i = 0; i < TL::LedManager::ledAnimator.size() && i < TL::LedManager::ledData.size(); i++)
	{
		TL::LedManager::ledAnimator.at(i)->render(TL::LedManager::ledData.at(i));
	}
	TL::LedManager::limitPowerConsumption();
	TL::LedManager::limitRegulatorTemperature();
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
 * @return OK when the LED data was created
 * @return ERROR_CREATE_LED_DATA when the LED data could not be created because the data could not be linked to a pin
 */
TL::LedManager::Error TL::LedManager::createLedData()
{
	TL::LedManager::ledData.assign(LED_NUM_ZONES, std::vector<CRGB>());
	for (size_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TL::Configuration::LedConfig ledConfig = TL::Configuration::getLedConfig(i);
		TL::LedManager::ledData.at(i).assign(ledConfig.ledCount, CRGB::Black);

		switch (ledConfig.ledPin)
		{
		case 13:
			FastLED.addLeds<NEOPIXEL, 13>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 14:
			FastLED.addLeds<NEOPIXEL, 14>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 15:
			FastLED.addLeds<NEOPIXEL, 15>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 16:
			FastLED.addLeds<NEOPIXEL, 16>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 17:
			FastLED.addLeds<NEOPIXEL, 17>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 21:
			FastLED.addLeds<NEOPIXEL, 21>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 22:
			FastLED.addLeds<NEOPIXEL, 22>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		case 25:
			FastLED.addLeds<NEOPIXEL, 25>(&TL::LedManager::ledData.at(i).front(), ledConfig.ledCount);
			break;
		default:
			return TL::LedManager::Error::ERROR_CREATE_LED_DATA;
		}
	}
	return TL::LedManager::Error::OK;
}

/**
 * @brief Create the LED animators based on the configuration.
 * @return OK when the animators were created
 * @return ERROR_UNKNOWN_ANIMATOR_TYPE when one of the animator types is unknown
 * @return ERROR_INVALID_FSEQ when a custom animation was set but the fseq file is invalid
 * @return ERROR_FILE_NOT_FOUND when the animation file was not found
 * @return ERROR_INVALID_LED_CONFIGURATION when the animation file is incompatible with the LED configuration
 */
TL::LedManager::Error TL::LedManager::createAnimators()
{
	// Custom animations will be used when the first animator type is set to 255
	// The used file identifier is set by the custom fields [10-13]
	// Field 14 is reserved to store the previous, calculated animation type
	const bool customAnimation = TL::Configuration::getLedConfig(0).type == 255;
	uint32_t identifier = 0;
	memcpy(&identifier, &TL::Configuration::getLedConfig(0).animationSettings[20], sizeof(identifier));
	if (!customAnimation)
	{
		return TL::LedManager::loadCalculatedAnimations();
	}
	else
	{
		String fileName;
		if (TL::FileUtil::getFileNameFromIdentifier(&SD, FSEQ_DIRECTORY, identifier, fileName) && fileName.length() > 0)
		{
			return TL::LedManager::loadCustomAnimation(fileName);
		}
		else
		{
			return TL::LedManager::Error::ERROR_FILE_NOT_FOUND;
		}
	}
}

/**
 * @brief Load animators for calculated animations.
 * @return OK when the calcualted animators were loaded
 * @return ERROR_UNKNOWN_ANIMATOR_TYPE when one of the animator types is unknown
 */
TL::LedManager::Error TL::LedManager::loadCalculatedAnimations()
{
	const size_t ledCount = TL::LedManager::getLedCount();
	if (ledCount <= 850)
	{
		// 60 FPS
		TL::LedManager::setRenderInterval(RENDER_INTERVAL);
		TL::LedManager::setFrameInterval(FRAME_INTERVAL);
	}
	else if (ledCount > 850 && ledCount <= 1000)
	{
		// 40 FPS
		TL::LedManager::setRenderInterval(RENDER_INTERVAL);
		TL::LedManager::setFrameInterval(FRAME_INTERVAL * 1.5f);
	}
	else
	{
		// 30 FPS
		TL::LedManager::setRenderInterval(RENDER_INTERVAL);
		TL::LedManager::setFrameInterval(FRAME_INTERVAL * 2.0f);
	}

	TL::LedManager::ledAnimator.resize(LED_NUM_ZONES);
	for (size_t i = 0; i < TL::LedManager::ledAnimator.size(); i++)
	{
		const TL::Configuration::LedConfig ledConfig = TL::Configuration::getLedConfig(i);

		// Rainbow type
		if (ledConfig.type == 0)
		{
			TL::LedManager::ledAnimator.at(i).reset(new TL::RainbowAnimator((TL::RainbowAnimator::RainbowMode)ledConfig.animationSettings[0]));
		}

		// Sparkle type
		else if (ledConfig.type == 1)
		{
			TL::LedManager::ledAnimator.at(i).reset(new TL::SparkleAnimator(
				static_cast<TL::SparkleAnimator::SpawnPosition>(ledConfig.animationSettings[0]),
				ledConfig.animationSettings[7] / 2 + 1,
				CRGB(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				ledConfig.animationSettings[8] / 5120.0f,
				ledConfig.animationSettings[9] / 2560.0f,
				ledConfig.animationSettings[10] / 255.0f,
				ledConfig.animationSettings[11] / 1024.0f,
				ledConfig.animationSettings[12] / 255.0f,
				ledConfig.animationSettings[13] / 255.0f,
				ledConfig.animationSettings[14] / 255.0f,
				ledConfig.animationSettings[15] / 5120.0f,
				ledConfig.animationSettings[16] / 2560.0f,
				ledConfig.animationSettings[17],
				ledConfig.animationSettings[18]));
		}

		// Gradient type
		else if (ledConfig.type == 2)
		{
			TL::LedManager::ledAnimator.at(i).reset(new TL::GradientAnimator(
				static_cast<TL::GradientAnimator::GradientMode>(ledConfig.animationSettings[0]),
				CRGB(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				CRGB(ledConfig.animationSettings[4], ledConfig.animationSettings[5], ledConfig.animationSettings[6])));
		}

		// Static color type
		else if (ledConfig.type == 3)
		{
			TL::LedManager::ledAnimator.at(i).reset(new TL::StaticColorAnimator(CRGB(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3])));
		}

		// Color bar type
		else if (ledConfig.type == 4)
		{
			TL::LedManager::ledAnimator.at(i).reset(new TL::ColorBarAnimator(
				static_cast<TL::ColorBarAnimator::ColorBarMode>(ledConfig.animationSettings[0]),
				CRGB(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				CRGB(ledConfig.animationSettings[4], ledConfig.animationSettings[5], ledConfig.animationSettings[6])));
		}

		// Rainbow motion type
		else if (ledConfig.type == 5)
		{
			TL::LedManager::ledAnimator.at(i).reset(new TL::RainbowAnimatorMotion(static_cast<TL::RainbowAnimatorMotion::RainbowMode>(ledConfig.animationSettings[0])));
		}

		// Gradient motion type
		else if (ledConfig.type == 6)
		{
			TL::LedManager::ledAnimator.at(i).reset(new TL::GradientAnimatorMotion(
				static_cast<TL::GradientAnimatorMotion::GradientMode>(ledConfig.animationSettings[0]),
				CRGB(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				CRGB(ledConfig.animationSettings[4], ledConfig.animationSettings[5], ledConfig.animationSettings[6])));
		}

		// Unknown type
		else
		{
			return TL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE;
		}

		TL::LedManager::ledAnimator.at(i)->setDataSource(static_cast<TL::LedAnimator::DataSource>(ledConfig.dataSource));
		TL::LedManager::ledAnimator.at(i)->setSpeed(ledConfig.speed);
		TL::LedManager::ledAnimator.at(i)->setOffset(ledConfig.offset);
		TL::LedManager::ledAnimator.at(i)->setAnimationBrightness(ledConfig.brightness / 255.0f);
		TL::LedManager::ledAnimator.at(i)->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		TL::LedManager::ledAnimator.at(i)->setReverse(ledConfig.reverse);
		TL::LedManager::ledAnimator.at(i)->init(TL::LedManager::ledData.at(i));
	}
	return TL::LedManager::Error::OK;
}

/**
 * @brief Load a custom animator and play the animation from the fseq loader.
 * @param fileName name of the fseq file to load
 * @return OK when the custom animation was loaded
 * @return ERROR_INVALID_FSEQ when a custom animation was set but the fseq file is invalid
 * @return ERROR_INVALID_LED_CONFIGURATION when the LED configuration is invalid for the custom animation
 */
TL::LedManager::Error TL::LedManager::loadCustomAnimation(const String &fileName)
{
	TL::LedManager::fseqLoader.reset(new TL::FseqLoader(&SD));
	const TL::FseqLoader::Error fseqError = TL::LedManager::fseqLoader->loadFromFile(FSEQ_DIRECTORY + (String)F("/") + fileName);
	if (fseqError != TL::FseqLoader::Error::OK)
	{
		TL::LedManager::fseqLoader.reset();
		return TL::LedManager::Error::ERROR_INVALID_FSEQ;
	}

	const uint32_t channelCount = TL::LedManager::getLedCount() * 3;
	const uint32_t roundedChannelCount = channelCount % 4 ? channelCount + (4 - channelCount % 4) : channelCount;
	const uint8_t fillerBytes = roundedChannelCount - channelCount;
	if (TL::LedManager::fseqLoader->getHeader().channelCount != roundedChannelCount)
	{
		return TL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION;
	}
	TL::LedManager::fseqLoader->setFillerBytes(fillerBytes);
	TL::LedManager::fseqLoader->setZoneCount(LED_NUM_ZONES);

	TL::LedManager::setRenderInterval(static_cast<uint32_t>(TL::LedManager::fseqLoader->getHeader().stepTime) * 1000);
	TL::LedManager::setFrameInterval(static_cast<uint32_t>(TL::LedManager::fseqLoader->getHeader().stepTime) * 1000);

	TL::LedManager::ledAnimator.resize(LED_NUM_ZONES);
	for (size_t i = 0; i < TL::LedManager::ledAnimator.size(); i++)
	{
		const TL::Configuration::LedConfig ledConfig = TL::Configuration::getLedConfig(i);
		TL::LedManager::ledAnimator.at(i).reset(new TL::FseqAnimator(TL::LedManager::fseqLoader.get(), true));
		TL::LedManager::ledAnimator.at(i)->setDataSource(static_cast<TL::LedAnimator::DataSource>(ledConfig.dataSource));
		TL::LedManager::ledAnimator.at(i)->setSpeed(ledConfig.speed);
		TL::LedManager::ledAnimator.at(i)->setOffset(ledConfig.offset);
		TL::LedManager::ledAnimator.at(i)->setAnimationBrightness(ledConfig.brightness / 255.0f);
		TL::LedManager::ledAnimator.at(i)->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		TL::LedManager::ledAnimator.at(i)->setReverse(ledConfig.reverse);
		TL::LedManager::ledAnimator.at(i)->init(TL::LedManager::ledData.at(i));
	}
	return TL::LedManager::Error::OK;
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

	for (size_t i = 0; i < TL::LedManager::ledData.size(); i++)
	{
		TL::Configuration::LedConfig ledConfig = TL::Configuration::getLedConfig(i);
		float zoneCurrent = 0.0f;
		for (size_t j = 0; j < TL::LedManager::ledData.at(i).size(); j++)
		{
			zoneCurrent += ledConfig.ledChannelCurrent[0] * TL::LedManager::ledData.at(i).at(j).r / 255.0f;
			zoneCurrent += ledConfig.ledChannelCurrent[1] * TL::LedManager::ledData.at(i).at(j).g / 255.0f;
			zoneCurrent += ledConfig.ledChannelCurrent[2] * TL::LedManager::ledData.at(i).at(j).b / 255.0f;
		}

		const uint8_t regulatorIndex = TL::LedManager::getRegulatorIndexFromPin(ledConfig.ledPin);
		regulatorPower[regulatorIndex] += zoneCurrent * ledConfig.ledVoltage / 1000.0f;
	}
}

/**
 * @brief Limit the power consumption of the current frame to the maxumim system power.
 */
void TL::LedManager::limitPowerConsumption()
{
	float regulatorPower[REGULATOR_COUNT];
	TL::LedManager::calculateRegulatorPowerDraw(regulatorPower);

	TL::Configuration::SystemConfig systemConfig = TL::Configuration::getSystemConfig();
	for (size_t i = 0; i < TL::LedManager::ledData.size(); i++)
	{
		TL::Configuration::LedConfig ledConfig = TL::Configuration::getLedConfig(i);
		const uint8_t regulatorIndex = TL::LedManager::getRegulatorIndexFromPin(ledConfig.ledPin);
		float multiplicator = ((float)systemConfig.regulatorPowerLimit / REGULATOR_COUNT) / regulatorPower[regulatorIndex];
		if (multiplicator < 0.0f)
		{
			multiplicator = 0.0f;
		}
		else if (multiplicator > 1.0f)
		{
			multiplicator = 1.0f;
		}

		for (size_t j = 0; j < TL::LedManager::ledData.at(i).size(); j++)
		{
			TL::LedManager::ledData.at(i).at(j).r *= multiplicator;
			TL::LedManager::ledData.at(i).at(j).g *= multiplicator;
			TL::LedManager::ledData.at(i).at(j).b *= multiplicator;
		}
	}
}

/**
 * @brief Limit the regulator temperature by reducing the LED brightness once the high temperature is reached.
 */
void TL::LedManager::limitRegulatorTemperature()
{
	float multiplicator = 1.0f - (TL::LedManager::regulatorTemperature - TL::Configuration::getSystemConfig().regulatorHighTemperature) / (TL::Configuration::getSystemConfig().regulatorCutoffTemperature - TL::Configuration::getSystemConfig().regulatorHighTemperature);
	if (multiplicator < 0.0f)
	{
		multiplicator = 0.0f;
	}
	else if (multiplicator > 1.0f)
	{
		multiplicator = 1.0f;
	}

	for (size_t i = 0; i < TL::LedManager::ledData.size(); i++)
	{
		for (size_t j = 0; j < TL::LedManager::ledData.at(i).size(); j++)
		{
			TL::LedManager::ledData.at(i).at(j).r *= multiplicator;
			TL::LedManager::ledData.at(i).at(j).g *= multiplicator;
			TL::LedManager::ledData.at(i).at(j).b *= multiplicator;
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
