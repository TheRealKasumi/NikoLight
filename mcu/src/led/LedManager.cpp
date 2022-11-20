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
 * @param config pointer to the configuration
 */
TesLight::LedManager::LedManager(TesLight::Configuration *config)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize LED manager."));
	this->config = config;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		this->ledData[i] = nullptr;
		this->ledAnimator[i] = nullptr;
	}
	this->fseqLoader = nullptr;
	this->targetFrameTime = LED_FRAME_TIME;
	this->regulatorTemperature = 0.0f;
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED mananger initialized."));
}

/**
 * @brief Destroy the {@link TesLight::LedManager} instance and clear memory.
 */
TesLight::LedManager::~LedManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Destroying LED manager."));
	this->clearAnimations();
}

/**
 * @brief Clear and create new LED data and animators from the configuration.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::reloadAnimations()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reload LEDs and animators from the configuration."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Clear old LED data and animators."));
	this->clearAnimations();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new LED data."));
	if (!this->createLedData())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create new LED data."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new animators."));
	if (!this->createAnimators())
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
void TesLight::LedManager::clearAnimations()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Clear old LED data and animators."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Unlink all LED data from the FastLED lib."));
	FastLED.clear();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete LED data and animators."));
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set ambient brightness."));
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
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
 * @brief Get the currently set ambient brightness.
 * @return float ambient brightness from 0.0 to 1.0
 */
bool TesLight::LedManager::getAmbientBrightness(float &ambientBrightness)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get ambient brightness."));
	if (this->ledAnimator[0] == nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to get ambient brightness value because led animator at index 0 is null."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Got the ambient brightness value."));
	ambientBrightness = this->ledAnimator[0]->getAmbientBrightness();
	return true;
}

/**
 * @brief Set the targeted frame time for rendering the LEDs.
 * The minimum frame time is currently limited to 13ms.
 * @param targetFrameTime target frame time in microseconds
 */
void TesLight::LedManager::setTargetFrameTime(const uint32_t targetFrameTime)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set target frame time."));
	this->targetFrameTime = targetFrameTime > 13 ? targetFrameTime : 13;
}

/**
 * @brief Get the targeted frame time for rendering the LEDs.
 * @return uint16_t targeted frame time in microseconds
 */
uint32_t TesLight::LedManager::getTargetFrameTime()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get target frame time."));
	return this->targetFrameTime;
}

/**
 * @brief Set the current motion sensor data.
 * @param motionSensorData instance of the {@link TesLight::MotionSensor::MotionSensorData}
 */
void TesLight::LedManager::setMotionSensorData(const TesLight::MotionSensor::MotionSensorData motionSensorData)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set motion sensor data."));
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		if (this->ledAnimator[i] != nullptr)
		{
			this->ledAnimator[i]->setMotionSensorData(motionSensorData);
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to set motion sensor data for animator ") + String(i) + F(" because the animator is null."));
		}
	}
}

/**
 * @brief Get the currently set and used motion sensor data.
 * @param motionSensorData reference to a {@link TesLight::MotionSensor::MotionSensorData}
 * @return true when the data was read successfully
 * @return false when there was an error
 */
bool TesLight::LedManager::getMotionSensorData(TesLight::MotionSensor::MotionSensorData &motionSensorData)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get motion sensor data."));
	if (this->ledAnimator[0] != nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to get motion sensor data because animator with index 0 was null."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Got motion sensor data."));
	motionSensorData = this->ledAnimator[0]->getMotionSensorData();
	return true;
}

/**
 * @brief Set the regulator temperature. Once a critical temperature is reached, the brightness will be reduced.
 * @param regulatorTemperature temperature of the generator in degree celsius.
 */
void TesLight::LedManager::setRegulatorTemperature(const float regulatorTemperature)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set regulator temperature."));
	this->regulatorTemperature = regulatorTemperature;
}

/**
 * @brief Get the currently set regulator temperature. This is not reading the temperature from the sensors.
 * @return currently set regulator temperature in degree celsius
 */
float TesLight::LedManager::getRegulatorTemperature()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get regulator temperature."));
	return this->regulatorTemperature;
}

/**
 * @brief Get the total power draw of all LEDs that has been calculated.
 * @return total power draw in W
 */
float TesLight::LedManager::getLedPowerDraw()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get total power draw of all LEDs."));
	float regulatorPower[REGULATOR_COUNT];
	if (!this->calculateRegulatorPowerDraw(regulatorPower))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to calculate power draw."));
		return 0.0f;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Sum up the power draw of each regulator."));
	float sum = 0.0f;
	for (uint8_t i = 0; i < REGULATOR_COUNT; i++)
	{
		sum += regulatorPower[i];
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Return total power draw."));
	return sum;
}

/**
 * @brief Render all LEDs using their animators.
 */
bool TesLight::LedManager::render()
{
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		if (this->ledAnimator[i] != nullptr)
		{
			this->ledAnimator[i]->render();
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to render LEDs with animator ") + String(i) + F(" because the animator is null."));
			return false;
		}
	}

	if (!this->limitPowerConsumption())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to limit LED power consumption after rendering."));
		return false;
	}

	if (!this->limitRegulatorTemperature())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to limit regulator temperature after rendering."));

		return false;
	}

	return true;
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
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::createLedData()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new LED data and link it to the FastLED lib."));
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);

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
		case 22:
			FastLED.addLeds<NEOPIXEL, 22>(ledData[i], ledConfig.ledCount);
			break;
		case 25:
			FastLED.addLeds<NEOPIXEL, 25>(ledData[i], ledConfig.ledCount);
			break;
		default:
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to link the created pixel data to the FastLED lib because the pin is invalid. It must be one of [13, 14, 15, 16, 17, 21, 22, 25]."));
			return false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("New LED data created and linked to the FastLED lib."));
	return true;
}

/**
 * @brief Create the LED animators based on the configuration.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::createAnimators()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create new LED animators."));

	// Custom animations will be used when the first animator type is set to 255
	// The used file identifier is set by the custom fields [10-13]
	// Field 14 is reserved to store the previous, calculated animation type
	const bool customAnimation = this->config->getLedConfig(0).type == 255;
	uint32_t identifier = 0;
	memcpy(&identifier, &this->config->getLedConfig(0).customField[10], sizeof(identifier));
	if (!customAnimation)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Using calculated animations."));

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Setting target frame time to ") + String(LED_FRAME_TIME) + F(" µs."));
		this->setTargetFrameTime(LED_FRAME_TIME);

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Loading calculated animators."));
		return this->loadCalculatedAnimations();
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Using fseq custom animation with animation file with id ") + String(identifier) + F("."));
		String fileName;
		if (TesLight::FileUtil::getFileNameFromIdentifier(&SD, FSEQ_DIRECTORY, identifier, fileName) && fileName.length() > 0)
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
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to determine file name for animation file with id ") + String(identifier) + F("."));
			return false;
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Setting target frame time to ") + String(this->fseqLoader->getHeader().stepTime * 1000) + F(" µs."));
		this->setTargetFrameTime((uint32_t)this->fseqLoader->getHeader().stepTime * 1000);

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Loading custom animators."));
		return this->loadCustomAnimation();
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Animators are loaded and ready to render."));
	return true;
}

/**
 * @brief Load animators for calculated animations.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::loadCalculatedAnimations()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Load animators for calculated animations."));
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);

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

		// Gradient linear type
		else if (ledConfig.type == 3)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Gradient linear."));
			ledAnimator[i] = new TesLight::GradientAnimator(TesLight::GradientAnimator::GradientMode::GRADIENT_LINEAR, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient center type
		else if (ledConfig.type == 4)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Gradient center."));
			ledAnimator[i] = new TesLight::GradientAnimator(TesLight::GradientAnimator::GradientMode::GRADIENT_CENTER, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Static color type
		else if (ledConfig.type == 5)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Static."));
			ledAnimator[i] = new TesLight::StaticColorAnimator(CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]));
		}

		// Color bar linear hard type
		else if (ledConfig.type == 6)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is COLOR_BAR_LINEAR_HARD."));
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Color bar linear smooth type
		else if (ledConfig.type == 7)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is COLOR_BAR_LINEAR_SMOOTH."));
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_SMOOTH, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Color bar center hard type
		else if (ledConfig.type == 8)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is COLOR_BAR_CENTER_HARD."));
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_HARD, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Color bar center smooth type
		else if (ledConfig.type == 9)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is COLOR_BAR_CENTER_SMOOTH."));
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_SMOOTH, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Rainbow linear motion acc x type
		else if (ledConfig.type == 10)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_LINEAR ACC_X_G."));
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_X_G);
		}

		// Rainbow linear motion acc y type
		else if (ledConfig.type == 11)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_LINEAR ACC_Y_G."));
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G);
		}

		// Rainbow linear motion acc x type
		else if (ledConfig.type == 10)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_LINEAR ACC_X_G."));
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_X_G);
		}

		// Rainbow linear motion acc y type
		else if (ledConfig.type == 11)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_LINEAR ACC_Y_G."));
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G);
		}

		// Rainbow center motion acc x type
		else if (ledConfig.type == 12)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_CENTER ACC_X_G."));
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_X_G);
		}

		// Rainbow center motion acc y type
		else if (ledConfig.type == 13)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is RAINBOW_CENTER ACC_Y_G."));
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G);
		}

		// Gradient linear motion acc x type
		else if (ledConfig.type == 14)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Gradient linear ACC_X_G."));
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_X_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient linear motion acc y type
		else if (ledConfig.type == 15)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Gradient linear ACC_Y_G."));
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient center motion acc x type
		else if (ledConfig.type == 16)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Gradient center ACC_X_G."));
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_X_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient center motion acc y type
		else if (ledConfig.type == 17)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is Gradient center ACC_Y_G."));
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
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
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::loadCustomAnimation()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Load animators for custom animations."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculating total number of LEDs."));
	uint32_t totalLedCount = 0;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
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
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
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

/**
 * @brief Calculate the total power draw from each regulator using the current frame.
 * @param regulatorPower array containing the power draw per regulator after the call
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT])
{
	for (uint8_t i = 0; i < REGULATOR_COUNT; i++)
	{
		regulatorPower[i] = 0.0f;
	}

	const uint8_t regulatorMap[LED_NUM_ZONES] = REGULATOR_ZONE_MAPPING;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		float zoneCurrent = 0.0f;
		if (this->ledAnimator[i] != nullptr)
		{
			for (uint16_t j = 0; j < this->ledAnimator[i]->getPixelCount(); j++)
			{
				zoneCurrent += ledConfig.ledChannelCurrent[0] * this->ledData[i][j].r / 255.0f;
				zoneCurrent += ledConfig.ledChannelCurrent[1] * this->ledData[i][j].g / 255.0f;
				zoneCurrent += ledConfig.ledChannelCurrent[2] * this->ledData[i][j].b / 255.0f;
			}
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to calculate power consumption for animator ") + String(i) + F(" because the animator is null."));
			return false;
		}
		regulatorPower[regulatorMap[i]] += zoneCurrent * (ledConfig.ledVoltage / 10.0f) / 1000.0f;
	}

	return true;
}

/**
 * @brief Limit the power consumption of the current frame to the maxumim system power.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::limitPowerConsumption()
{
	float regulatorPower[REGULATOR_COUNT];
	if (!this->calculateRegulatorPowerDraw(regulatorPower))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to get regulator power draw."));
		return false;
	}

	const uint8_t regulatorMap[LED_NUM_ZONES] = REGULATOR_ZONE_MAPPING;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		TesLight::Configuration::SystemConfig systemConfig = this->config->getSystemConfig();
		if (this->ledAnimator[i] != nullptr)
		{
			float multiplicator = ((float)systemConfig.regulatorPowerLimit / REGULATOR_COUNT) / regulatorPower[regulatorMap[i]];
			if (multiplicator < 0.0f)
			{
				multiplicator = 0.0f;
			}
			else if (multiplicator > 1.0f)
			{
				multiplicator = 1.0f;
			}

			for (uint16_t j = 0; j < this->ledAnimator[i]->getPixelCount(); j++)
			{
				this->ledData[i][j].r *= multiplicator;
				this->ledData[i][j].g *= multiplicator;
				this->ledData[i][j].b *= multiplicator;
			}
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to limit power consumption for animator ") + String(i) + F(" because the animator is null."));
			return false;
		}
	}

	return true;
}

/**
 * @brief Limit the regulator temperature by reducing the LED brightness once the high temperature is reached.
 * @return true when successul
 * @return false when there was an error
 */
bool TesLight::LedManager::limitRegulatorTemperature()
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

	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		if (this->ledAnimator[i] != nullptr)
		{
			for (uint16_t j = 0; j < this->ledAnimator[i]->getPixelCount(); j++)
			{
				this->ledData[i][j].r *= multiplicator;
				this->ledData[i][j].g *= multiplicator;
				this->ledData[i][j].b *= multiplicator;
			}
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to limit regulator temperature because animator with index ") + String(i) + F(" is null."));
			return false;
		}
	}

	return true;
}