/**
 * @file LedManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::LedManager}.
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "led/LedManager.h"

/**
 * @brief Create a new instance of {@link TesLight::LedManager}.
 */
TesLight::LedManager::LedManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize LedManager."));
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		this->ledData[i] = nullptr;
		this->ledAnimator[i] = nullptr;
	}
	this->fseqLoader = nullptr;
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LedMananger initialized."));
}

/**
 * @brief Destroy the {@link TesLight::LedManager} instance and clear memory.
 */
TesLight::LedManager::~LedManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Destroying LedManager."));
	this->clear();
}

/**
 * @brief Clear and create new LED data and animators based on the configuration.
 * @param config instsance of {@link TesLight::Configuration}
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::loadFromConfiguration(TesLight::Configuration *config)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Load LEDs and animators from configuration."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Clear old LED data and animators."));
	this->clear();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new LED data."));
	if (!this->createLedData(config))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create new LED data."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new animators."));
	if (!this->createAnimators(config))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create new animators."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LEDs and animators loaded from configuration."));
	return true;
}

/**
 * @brief Clear the LED data and animators to free memory.
 */
void TesLight::LedManager::clear()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Clear old LED data and animators."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Unlink all LED data from the FastLED lib."));
	FastLED.clear();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete LED data and animators."));
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		if (this->ledData[i] != nullptr)
		{
			delete[] this->ledData[i];
			this->ledData[i] = nullptr;
		}
		if (this->ledAnimator[i] != nullptr)
		{
			delete this->ledAnimator[i];
			this->ledAnimator[i] = nullptr;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete FseqLoader."));
	if (this->fseqLoader != nullptr)
	{
		delete this->fseqLoader;
		this->fseqLoader = nullptr;
	}
}

/**
 * @brief Set the current ambient brightness for all underlying LED animators.
 * @param ambientBrightness ambient brightness from 0.0 to 1.0
 */
void TesLight::LedManager::setAmbientBrightness(const float ambientBrightness)
{
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		if (this->ledAnimator[i] != nullptr)
		{
			this->ledAnimator[i]->setAmbientBrightness(ambientBrightness);
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to set ambient brightness for animator ") + String(i) + F(" because the animator is null."));
		}
	}
}

/**
 * @brief Render all LEDs using their animators.
 */
void TesLight::LedManager::render()
{
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		if (this->ledAnimator[i] != nullptr)
		{
			this->ledAnimator[i]->render();
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to render LEDs with animator ") + String(i) + F(" because the animator is null."));
		}
	}
}

/**
 * @brief Show the current LED data.
 */
void TesLight::LedManager::show()
{
	FastLED.show();
}

/**
 * @brief Create the LED data and assign it to the FastLED library.
 * @param config instance of {@TesLight::Configuration}
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::createLedData(TesLight::Configuration *config)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new LED data and link it to the FastLED lib."));
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = config->getLedConfig(i);

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Allocate pixel data for ") + String(ledConfig.ledCount) + F(" LEDs at pin ") + String(ledConfig.ledPin) + F(" and initialize it to black."));
		ledData[i] = new CRGB[ledConfig.ledCount];
		for (uint16_t j = 0; j < ledConfig.ledCount; j++)
		{
			ledData[i][j] = CRGB::Black;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Link the created pixel data to the FastLED lib."));
		switch (ledConfig.ledPin)
		{
		case 13:
			FastLED.addLeds<NEOPIXEL, 13>(ledData[i], ledConfig.ledCount);
			break;
		case 14:
			FastLED.addLeds<NEOPIXEL, 14>(ledData[i], ledConfig.ledCount);
			break;
		case 15:
			FastLED.addLeds<NEOPIXEL, 15>(ledData[i], ledConfig.ledCount);
			break;
		case 16:
			FastLED.addLeds<NEOPIXEL, 16>(ledData[i], ledConfig.ledCount);
			break;
		case 17:
			FastLED.addLeds<NEOPIXEL, 17>(ledData[i], ledConfig.ledCount);
			break;
		case 21:
			FastLED.addLeds<NEOPIXEL, 21>(ledData[i], ledConfig.ledCount);
			break;
		default:
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to link the created pixel data to the FastLED lib because the pin is invalid. It must be one of [13, 14, 15, 16, 17, 21]."));
			return false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("New LED data created and linked to the FastLED lib."));
	return true;
}

/**
 * @brief Create the LED animators based on the configuration.
 * @param config instance of {@TesLight::Configuration}
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::createAnimators(TesLight::Configuration *config)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new LED animators."));

	// Custom animations will be used when the first animator type is set to 255
	// The used file index is set by the first custom field
	const bool customAnimation = config->getLedConfig(0).type == 255;
	const uint8_t animationFileIndex = config->getLedConfig(0).customField[0];
	if (!customAnimation)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Using calculated animations."));
		return this->loadCalculatedAnimations(config);
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Using fseq custom animation with animation file index ") + String(animationFileIndex) + F("."));
		String fileName;
		TesLight::FileUtil::getFileNameByIndex(&SD, FSEQ_DIRECTORY, animationFileIndex, fileName);
		if (fileName.length() > 0)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Loading fseq file: ") + fileName);
			this->fseqLoader = new TesLight::FseqLoader(&SD);
			if (!this->fseqLoader->loadFromFile(FSEQ_DIRECTORY + (String)F("/") + fileName))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load fseq file. Cleaning FseqLoader."));
				delete this->fseqLoader;
				return false;
			}
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Fseq file loaded and ready to be streamed."));
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to determine file name for animation file index ") + String(animationFileIndex) + F("."));
			return false;
		}

		return this->loadCustomAnimation(config);
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Animators are loaded and ready to render."));
	return true;
}

/**
 * @brief Load animators for calculated animations.
 * @param config instance of {@link TesLight::Configuration}
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::loadCalculatedAnimations(TesLight::Configuration *config)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Load animators for calculated animations."));
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = config->getLedConfig(i);

		// Rainbow solid type
		if (ledConfig.type == 0)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_SOLID."));
			ledAnimator[i] = new TesLight::RainbowAnimator(TesLight::RainbowAnimator::RainbowMode::RAINBOW_SOLID);
		}

		// Rainbow linear type
		else if (ledConfig.type == 1)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_LINEAR."));
			ledAnimator[i] = new TesLight::RainbowAnimator(TesLight::RainbowAnimator::RainbowMode::RAINBOW_LINEAR);
		}

		// Rainbow middle type
		else if (ledConfig.type == 2)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_CENTER."));
			ledAnimator[i] = new TesLight::RainbowAnimator(TesLight::RainbowAnimator::RainbowMode::RAINBOW_CENTER);
		}

		// Gradient type
		else if (ledConfig.type == 3)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Gradient."));
			ledAnimator[i] = new TesLight::GradientAnimator(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2], ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]);
		}

		// Static color type
		else if (ledConfig.type == 4)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Static."));
			ledAnimator[i] = new TesLight::StaticColorAnimator(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]);
		}

		// Unknown type
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is unknown. Invalid value ") + String(ledConfig.type) + F("."));
			return false;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set properties for animator ") + String(i) + F("."));
		ledAnimator[i]->setPixels(this->ledData[i]);
		ledAnimator[i]->setPixelCount(ledConfig.ledCount);
		ledAnimator[i]->setSpeed(ledConfig.speed);
		ledAnimator[i]->setOffset(ledConfig.offset);
		ledAnimator[i]->setAnimationBrightness(ledConfig.brightness / 255.0f);
		ledAnimator[i]->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		ledAnimator[i]->setReverse(ledConfig.reverse);
		ledAnimator[i]->init();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Animators for calculated animations loaded."));
	return true;
}

/**
 * @brief Load a custom animator and play the animation from the fseq loader.
 * @param config instance of {@link TesLight::Configuration}
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::loadCustomAnimation(TesLight::Configuration *config)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Load animators for custom animations."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculating total number of LEDs."));
	uint32_t totalLedCount = 0;
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = config->getLedConfig(i);
		totalLedCount += ledConfig.ledCount;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Total number of LEDs is: ") + String(totalLedCount));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Check if the fseq files contains a suitable number of channels for the current LED configuration."));
	if (this->fseqLoader->getHeader().channelCount != totalLedCount * 3)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file can not be used with the current LED configuration because the LED count doesn't match the channel count."));
		return false;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Fseq file is usable."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create LED animators for fseq custom animation."));
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = config->getLedConfig(i);
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Create LED animator of type fseq for ") + ledConfig.ledCount + F(" LED's on pin ") + ledConfig.ledPin);

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Set properties for animator ") + String(i) + F("."));
		ledAnimator[i] = new TesLight::FseqAnimator(this->fseqLoader, true);
		ledAnimator[i]->setPixels(this->ledData[i]);
		ledAnimator[i]->setPixelCount(ledConfig.ledCount);
		ledAnimator[i]->setSpeed(ledConfig.speed);
		ledAnimator[i]->setOffset(ledConfig.offset);
		ledAnimator[i]->setAnimationBrightness(ledConfig.brightness / 255.0f);
		ledAnimator[i]->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		ledAnimator[i]->setReverse(ledConfig.reverse);
		ledAnimator[i]->init();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Animators for custom animation loaded."));
	return true;
}