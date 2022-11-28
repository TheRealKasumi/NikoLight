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
	this->config = config;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		this->ledData[i] = nullptr;
		this->ledAnimator[i] = nullptr;
	}
	this->fseqLoader = nullptr;
	this->targetFrameTime = LED_FRAME_TIME;
	this->regulatorTemperature = 0.0f;
}

/**
 * @brief Destroy the {@link TesLight::LedManager} instance and clear memory.
 */
TesLight::LedManager::~LedManager()
{
	this->clearAnimations();
}

/**
 * @brief Clear and create new LED data and animators from the configuration.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::reloadAnimations()
{
	this->clearAnimations();

	if (!this->createLedData())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create new LED data."));
		return false;
	}

	if (!this->createAnimators())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create new animators."));
		return false;
	}

	return true;
}

/**
 * @brief Clear the LED data and animators to free memory.
 */
void TesLight::LedManager::clearAnimations()
{
	FastLED.clear();

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
	if (this->ledAnimator[0] == nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to get ambient brightness value because led animator at index 0 is null."));
		return false;
	}

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
	this->targetFrameTime = targetFrameTime > 13 ? targetFrameTime : 13;
}

/**
 * @brief Get the targeted frame time for rendering the LEDs.
 * @return uint16_t targeted frame time in microseconds
 */
uint32_t TesLight::LedManager::getTargetFrameTime()
{
	return this->targetFrameTime;
}

/**
 * @brief Set the current motion sensor data.
 * @param motionSensorData instance of the {@link TesLight::MotionSensor::MotionSensorData}
 */
void TesLight::LedManager::setMotionSensorData(const TesLight::MotionSensor::MotionSensorData motionSensorData)
{
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
	if (this->ledAnimator[0] != nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to get motion sensor data because animator with index 0 was null."));
		return false;
	}

	motionSensorData = this->ledAnimator[0]->getMotionSensorData();
	return true;
}

/**
 * @brief Set the regulator temperature. Once a critical temperature is reached, the brightness will be reduced.
 * @param regulatorTemperature temperature of the generator in degree celsius.
 */
void TesLight::LedManager::setRegulatorTemperature(const float regulatorTemperature)
{
	this->regulatorTemperature = regulatorTemperature;
}

/**
 * @brief Get the currently set regulator temperature. This is not reading the temperature from the sensors.
 * @return currently set regulator temperature in degree celsius
 */
float TesLight::LedManager::getRegulatorTemperature()
{
	return this->regulatorTemperature;
}

/**
 * @brief Get the total power draw of all LEDs that has been calculated.
 * @return total power draw in W
 */
float TesLight::LedManager::getLedPowerDraw()
{
	float regulatorPower[REGULATOR_COUNT];
	if (!this->calculateRegulatorPowerDraw(regulatorPower))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to calculate power draw."));
		return 0.0f;
	}

	float sum = 0.0f;
	for (uint8_t i = 0; i < REGULATOR_COUNT; i++)
	{
		sum += regulatorPower[i];
	}

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
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		ledData[i] = new CRGB[ledConfig.ledCount];
		for (uint16_t j = 0; j < ledConfig.ledCount; j++)
		{
			ledData[i][j] = CRGB::Black;
		}

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

	return true;
}

/**
 * @brief Create the LED animators based on the configuration.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::createAnimators()
{
	// Custom animations will be used when the first animator type is set to 255
	// The used file identifier is set by the custom fields [10-13]
	// Field 14 is reserved to store the previous, calculated animation type
	const bool customAnimation = this->config->getLedConfig(0).type == 255;
	uint32_t identifier = 0;
	memcpy(&identifier, &this->config->getLedConfig(0).customField[10], sizeof(identifier));
	if (!customAnimation)
	{
		this->setTargetFrameTime(LED_FRAME_TIME);
		return this->loadCalculatedAnimations();
	}
	else
	{
		String fileName;
		if (TesLight::FileUtil::getFileNameFromIdentifier(&SD, FSEQ_DIRECTORY, identifier, fileName) && fileName.length() > 0)
		{
			this->fseqLoader = new TesLight::FseqLoader(&SD);
			if (!this->fseqLoader->loadFromFile(FSEQ_DIRECTORY + (String)F("/") + fileName))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load fseq file. Cleaning FseqLoader."));
				delete this->fseqLoader;
				return false;
			}
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to determine file name for animation file with id ") + String(identifier) + F("."));
			return false;
		}

		this->setTargetFrameTime((uint32_t)this->fseqLoader->getHeader().stepTime * 1000);
		return this->loadCustomAnimation();
	}

	return true;
}

/**
 * @brief Load animators for calculated animations.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::loadCalculatedAnimations()
{
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);

		// Rainbow solid type
		if (ledConfig.type == 0)
		{
			ledAnimator[i] = new TesLight::RainbowAnimator(TesLight::RainbowAnimator::RainbowMode::RAINBOW_SOLID);
		}

		// Rainbow linear type
		else if (ledConfig.type == 1)
		{
			ledAnimator[i] = new TesLight::RainbowAnimator(TesLight::RainbowAnimator::RainbowMode::RAINBOW_LINEAR);
		}

		// Rainbow middle type
		else if (ledConfig.type == 2)
		{
			ledAnimator[i] = new TesLight::RainbowAnimator(TesLight::RainbowAnimator::RainbowMode::RAINBOW_CENTER);
		}

		// Gradient linear type
		else if (ledConfig.type == 3)
		{
			ledAnimator[i] = new TesLight::GradientAnimator(TesLight::GradientAnimator::GradientMode::GRADIENT_LINEAR, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient center type
		else if (ledConfig.type == 4)
		{
			ledAnimator[i] = new TesLight::GradientAnimator(TesLight::GradientAnimator::GradientMode::GRADIENT_CENTER, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Static color type
		else if (ledConfig.type == 5)
		{
			ledAnimator[i] = new TesLight::StaticColorAnimator(CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]));
		}

		// Color bar linear hard type
		else if (ledConfig.type == 6)
		{
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_HARD, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Color bar linear smooth type
		else if (ledConfig.type == 7)
		{
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_LINEAR_SMOOTH, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Color bar center hard type
		else if (ledConfig.type == 8)
		{
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_HARD, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Color bar center smooth type
		else if (ledConfig.type == 9)
		{
			ledAnimator[i] = new TesLight::ColorBarAnimator(TesLight::ColorBarAnimator::ColorBarMode::COLOR_BAR_CENTER_SMOOTH, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Rainbow linear motion acc x type
		else if (ledConfig.type == 10)
		{
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_X_G);
		}

		// Rainbow linear motion acc y type
		else if (ledConfig.type == 11)
		{
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G);
		}

		// Rainbow linear motion acc x type
		else if (ledConfig.type == 10)
		{
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_X_G);
		}

		// Rainbow linear motion acc y type
		else if (ledConfig.type == 11)
		{
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G);
		}

		// Rainbow center motion acc x type
		else if (ledConfig.type == 12)
		{
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_X_G);
		}

		// Rainbow center motion acc y type
		else if (ledConfig.type == 13)
		{
			ledAnimator[i] = new TesLight::RainbowAnimatorMotion(TesLight::RainbowAnimatorMotion::RainbowMode::RAINBOW_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G);
		}

		// Gradient linear motion acc x type
		else if (ledConfig.type == 14)
		{
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_X_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient linear motion acc y type
		else if (ledConfig.type == 15)
		{
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_LINEAR, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient center motion acc x type
		else if (ledConfig.type == 16)
		{
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_X_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Gradient center motion acc y type
		else if (ledConfig.type == 17)
		{
			ledAnimator[i] = new TesLight::GradientAnimatorMotion(TesLight::GradientAnimatorMotion::GradientMode::GRADIENT_CENTER, TesLight::MotionSensor::MotionSensorValue::ACC_Y_G, CRGB(ledConfig.customField[0], ledConfig.customField[1], ledConfig.customField[2]), CRGB(ledConfig.customField[3], ledConfig.customField[4], ledConfig.customField[5]));
		}

		// Unknown type
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Animator type for animator ") + String(i) + F(" is unknown. Invalid value ") + String(ledConfig.type) + F("."));
			return false;
		}

		ledAnimator[i]->setPixels(this->ledData[i]);
		ledAnimator[i]->setPixelCount(ledConfig.ledCount);
		ledAnimator[i]->setSpeed(ledConfig.speed);
		ledAnimator[i]->setOffset(ledConfig.offset);
		ledAnimator[i]->setAnimationBrightness(ledConfig.brightness / 255.0f);
		ledAnimator[i]->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		ledAnimator[i]->setReverse(ledConfig.reverse);
		ledAnimator[i]->init();
	}

	return true;
}

/**
 * @brief Load a custom animator and play the animation from the fseq loader.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedManager::loadCustomAnimation()
{
	uint32_t totalLedCount = 0;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		totalLedCount += ledConfig.ledCount;
	}

	if (this->fseqLoader->getHeader().channelCount != totalLedCount * 3)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The fseq file can not be used with the current LED configuration because the LED count doesn't match the channel count."));
		return false;
	}

	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
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

		const uint8_t regulatorIndex = this->getRegulatorIndexFromPin(ledConfig.ledPin);
		regulatorPower[regulatorIndex] += zoneCurrent * (ledConfig.ledVoltage / 10.0f) / 1000.0f;
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

	TesLight::Configuration::SystemConfig systemConfig = this->config->getSystemConfig();
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		TesLight::Configuration::LedConfig ledConfig = this->config->getLedConfig(i);
		if (this->ledAnimator[i] != nullptr)
		{
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

/**
 * @brief Get the regulator index by providing the pin number.
 * @param pin physical pin number
 * @return regulator index
 */
uint8_t TesLight::LedManager::getRegulatorIndexFromPin(const uint8_t pin)
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