/**
 * @file LedManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::LedManager}.
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

#include "led/LedManager.h"

bool NL::LedManager::initialized = false;
std::unique_ptr<NL::LedBuffer> NL::LedManager::ledBuffer;
std::vector<std::unique_ptr<NL::LedAnimator>> NL::LedManager::ledAnimator;
std::unique_ptr<NL::FseqLoader> NL::LedManager::fseqLoader;
uint32_t NL::LedManager::frameInterval;
float NL::LedManager::regulatorTemperature;

/**
 * @brief Start the LED manager.
 * @return OK when the LED manager was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration was not initialized
 */
NL::LedManager::Error NL::LedManager::begin()
{
	NL::LedManager::initialized = false;
	NL::LedManager::frameInterval = FRAME_INTERVAL;
	NL::LedManager::regulatorTemperature = 0.0f;

	if (!NL::Configuration::isInitialized())
	{
		return NL::LedManager::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	NL::LedManager::initialized = true;
	return NL::LedManager::Error::OK;
}

/**
 * @brief Stop the LED manager.
 */
void NL::LedManager::end()
{
	NL::LedManager::clearAnimations();
}

/**
 * @brief Check if the LED manager is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::LedManager::isInitialized()
{
	return NL::LedManager::initialized;
}

/**
 * @brief Clear and create new LED data and animators from the configuration.
 * @return OK when the animation were reloaded
 * @return ERROR_INIT_LED_DRIVER when the LED data could not be created
 * @return ERROR_UNKNOWN_ANIMATOR_TYPE when one of the animator types is unknown
 * @return ERROR_INVALID_FSEQ when a custom animation was set but the fseq file is invalid
 * @return ERROR_FILE_NOT_FOUND when the animation file was not found
 * @return ERROR_INVALID_LED_CONFIGURATION when the current LED configuration does not match the custom animation
 */
NL::LedManager::Error NL::LedManager::reloadAnimations()
{
	NL::LedManager::clearAnimations();

	const NL::LedManager::Error ledDataError = NL::LedManager::initLedDriver();
	if (ledDataError != NL::LedManager::Error::OK)
	{
		return ledDataError;
	}

	const NL::LedManager::Error animatorError = NL::LedManager::createAnimators();
	if (animatorError != NL::LedManager::Error::OK)
	{
		return animatorError;
	}

	return NL::LedManager::Error::OK;
}

/**
 * @brief Clear the LED data and animators to free memory.
 */
void NL::LedManager::clearAnimations()
{
	NL::LedDriver::end();
	NL::LedManager::ledBuffer.reset();
	NL::LedManager::ledAnimator.clear();
	NL::LedManager::fseqLoader.reset();
}

/**
 * @brief Set the current ambient brightness for all underlying LED animators.
 * @param ambientBrightness ambient brightness from 0.0 to 1.0
 */
void NL::LedManager::setAmbientBrightness(const float ambientBrightness)
{
	for (size_t i = 0; i < NL::LedManager::ledAnimator.size(); i++)
	{
		NL::LedManager::ledAnimator.at(i)->setAmbientBrightness(ambientBrightness);
	}
}

/**
 * @brief Set the interval for outputting to the LEDs in µs.
 * The minimum frame time is currently limited to 10000µs.
 * @param frameInterval target frame time in µs
 */
void NL::LedManager::setFrameInterval(const uint32_t frameInterval)
{
	NL::LedManager::frameInterval = frameInterval >= 10000 ? frameInterval : 10000;
}

/**
 * @brief Get the interval for outputting to the LEDs.
 * @return interval in microseconds
 */
uint32_t NL::LedManager::getFrameInterval()
{
	return NL::LedManager::frameInterval;
}

/**
 * @brief Set the current motion sensor data.
 * @param motionSensorData motion sensor data
 */
void NL::LedManager::setMotionSensorData(const NL::MotionSensor::MotionSensorData &motionSensorData)
{
	for (size_t i = 0; i < NL::LedManager::ledAnimator.size(); i++)
	{
		NL::LedManager::ledAnimator.at(i)->setMotionSensorData(motionSensorData);
	}
}

/**
 * @brief Set the current audio analysis data.
 * @param audioAnalysis audio analysis data
 */
void NL::LedManager::setAudioAnalysis(const NL::AudioUnit::AudioAnalysis &audioAnalysis)
{
	for (size_t i = 0; i < NL::LedManager::ledAnimator.size(); i++)
	{
		NL::LedManager::ledAnimator.at(i)->setAudioAnalysis(audioAnalysis);
	}
}

/**
 * @brief Set the regulator temperature. Once a critical temperature is reached, the brightness will be reduced.
 * @param regulatorTemperature temperature of the generator in degree celsius.
 */
void NL::LedManager::setRegulatorTemperature(const float regulatorTemperature)
{
	NL::LedManager::regulatorTemperature = regulatorTemperature;
}

/**
 * @brief Get the total power draw of all LEDs that has been calculated.
 * @return total power draw in W
 */
float NL::LedManager::getLedPowerDraw()
{
	float regulatorPower[REGULATOR_COUNT];
	NL::LedManager::calculateRegulatorPowerDraw(regulatorPower);

	float sum = 0.0f;
	for (uint8_t i = 0; i < REGULATOR_COUNT; i++)
	{
		sum += regulatorPower[i];
	}
	return sum;
}

/**
 * @brief Get the total number of visible LEDs.
 * @return total number of LEDs
 */
size_t NL::LedManager::getLedCount()
{
	if (NL::LedManager::ledBuffer != nullptr)
	{
		return NL::LedManager::ledBuffer->getTotalLedCount();
	}
	return 0;
}

/**
 * @brief Get the total number of LEDs.
 * @return total number of LEDs
 */
size_t NL::LedManager::getHiddenLedCount()
{
	if (NL::LedManager::ledBuffer != nullptr)
	{
		return NL::LedManager::ledBuffer->getTotalHiddenLedCount();
	}
	return 0;
}

/**
 * @brief Render all LEDs using their animators.
 */
void NL::LedManager::render()
{
	if (NL::LedManager::ledBuffer == nullptr || !NL::LedDriver::isInitialized())
	{
		return;
	}

	for (size_t i = 0; i < NL::LedManager::ledBuffer->getLedStripCount(); i++)
	{
		NL::LedManager::ledAnimator.at(i)->render(NL::LedManager::ledBuffer->getLedStrip(i));
	}

	NL::LedManager::limitPowerConsumption();
	NL::LedManager::limitRegulatorTemperature();
}

/**
 * @brief Wait for the LED driver until all data was sent out to the LEDs.
 * @param timeout cpu cycles until a timeout will happen when data is still being send
 * @return OK when the driver is done sending data
 * @return ERROR_DRIVER_NOT_READY when the driver is not ready sending all the data
 */
NL::LedManager::Error NL::LedManager::waitShow(const TickType_t timeout)
{
	if (!NL::LedDriver::isInitialized())
	{
		return NL::LedManager::Error::ERROR_DRIVER_NOT_READY;
	}

	const NL::LedDriver::Error driverError = NL::LedDriver::isReady(timeout);
	if (driverError != NL::LedDriver::Error::OK)
	{
		return NL::LedManager::Error::ERROR_DRIVER_NOT_READY;
	}

	return NL::LedManager::Error::OK;
}

/**
 * @brief Async send out the current LED data via the LED driver.
 * @param timeout cpu cycles until a timeout will happen when data is still being send
 * @return OK when the data is being sent
 * @return ERROR_DRIVER_NOT_READY when the driver is not ready to send new data
 */
NL::LedManager::Error NL::LedManager::show(const TickType_t timeout)
{
	if (NL::LedManager::ledBuffer == nullptr || !NL::LedDriver::isInitialized())
	{
		return NL::LedManager::Error::ERROR_DRIVER_NOT_READY;
	}

	const NL::LedDriver::Error driverError = NL::LedDriver::showPixels(timeout);
	if (driverError != NL::LedDriver::Error::OK)
	{
		return NL::LedManager::Error::ERROR_DRIVER_NOT_READY;
	}

	return NL::LedManager::Error::OK;
}

/**
 * @brief Initialize the LED driver, buffer and output channels.
 * @return OK when the LED data was created
 * @return ERROR_INIT_LED_DRIVER when the LED data could not be created
 */
NL::LedManager::Error NL::LedManager::initLedDriver()
{
	std::vector<NL::LedStrip> ledStrips;
	for (size_t i = 0; i < LED_NUM_ZONES; i++)
	{
		NL::Configuration::LedConfig ledConfig;
		NL::Configuration::getLedConfig(i, ledConfig);
		ledStrips.push_back(NL::LedStrip(ledConfig.ledPin, ledConfig.ledCount, LED_MAX_COUNT_PER_ZONE));
	}

	NL::LedManager::ledBuffer.reset(new NL::LedBuffer(ledStrips));
	NL::LedDriver::Error driverError = NL::LedDriver::begin(*NL::LedManager::ledBuffer);
	if (driverError != NL::LedDriver::Error::OK)
	{
		return NL::LedManager::Error::ERROR_INIT_LED_DRIVER;
	}

	return NL::LedManager::Error::OK;
}

/**
 * @brief Create the LED animators based on the configuration.
 * @return OK when the animators were created
 * @return ERROR_UNKNOWN_ANIMATOR_TYPE when one of the animator types is unknown
 * @return ERROR_INVALID_FSEQ when a custom animation was set but the fseq file is invalid
 * @return ERROR_FILE_NOT_FOUND when the animation file was not found
 * @return ERROR_INVALID_LED_CONFIGURATION when the animation file is incompatible with the LED configuration
 */
NL::LedManager::Error NL::LedManager::createAnimators()
{
	// Custom animations will be used when the first animator type is set to 255
	// The used file identifier is set by the custom fields [10-13]
	// Field 14 is reserved to store the previous, calculated animation type
	NL::Configuration::LedConfig ledConfig;
	NL::Configuration::getLedConfig(0, ledConfig);

	const bool customAnimation = ledConfig.type == 255;
	uint32_t identifier = 0;
	std::memcpy(&identifier, &ledConfig.animationSettings[20], sizeof(identifier));
	if (!customAnimation)
	{
		return NL::LedManager::loadCalculatedAnimations();
	}
	else
	{
		String fileName;
		if (NL::FileUtil::getFileNameFromIdentifier(&SD, FSEQ_DIRECTORY, identifier, fileName) && fileName.length() > 0)
		{
			return NL::LedManager::loadCustomAnimation(fileName);
		}
		else
		{
			return NL::LedManager::Error::ERROR_FILE_NOT_FOUND;
		}
	}
}

/**
 * @brief Load animators for calculated animations.
 * @return OK when the calcualted animators were loaded
 * @return ERROR_UNKNOWN_ANIMATOR_TYPE when one of the animator types is unknown
 */
NL::LedManager::Error NL::LedManager::loadCalculatedAnimations()
{
	NL::LedManager::setFrameInterval(FRAME_INTERVAL);
	NL::LedManager::ledAnimator.resize(LED_NUM_ZONES);
	for (size_t i = 0; i < NL::LedManager::ledAnimator.size(); i++)
	{
		NL::Configuration::LedConfig ledConfig;
		NL::Configuration::getLedConfig(i, ledConfig);

		// Rainbow type
		if (ledConfig.type == 0)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::RainbowAnimator((NL::RainbowAnimator::RainbowMode)ledConfig.animationSettings[0]));
		}

		// Sparkle type
		else if (ledConfig.type == 1)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::SparkleAnimator(
				static_cast<NL::SparkleAnimator::SpawnPosition>(ledConfig.animationSettings[0]),
				ledConfig.animationSettings[7] / 2 + 1,
				NL::Pixel(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
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
			NL::LedManager::ledAnimator.at(i).reset(new NL::GradientAnimator(
				static_cast<NL::GradientAnimator::GradientMode>(ledConfig.animationSettings[0]),
				NL::Pixel(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				NL::Pixel(ledConfig.animationSettings[4], ledConfig.animationSettings[5], ledConfig.animationSettings[6])));
		}

		// Static color type
		else if (ledConfig.type == 3)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::StaticColorAnimator(NL::Pixel(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3])));
		}

		// Color bar type
		else if (ledConfig.type == 4)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::ColorBarAnimator(
				static_cast<NL::ColorBarAnimator::ColorBarMode>(ledConfig.animationSettings[0]),
				NL::Pixel(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				NL::Pixel(ledConfig.animationSettings[4], ledConfig.animationSettings[5], ledConfig.animationSettings[6])));
		}

		// Rainbow motion type
		else if (ledConfig.type == 5)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::RainbowAnimatorMotion(static_cast<NL::RainbowAnimatorMotion::RainbowMode>(ledConfig.animationSettings[0])));
		}

		// Gradient motion type
		else if (ledConfig.type == 6)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::GradientAnimatorMotion(
				static_cast<NL::GradientAnimatorMotion::GradientMode>(ledConfig.animationSettings[0]),
				NL::Pixel(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				NL::Pixel(ledConfig.animationSettings[4], ledConfig.animationSettings[5], ledConfig.animationSettings[6])));
		}

		// Pulse type
		else if (ledConfig.type == 7)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::PulseAnimator(
				static_cast<NL::PulseAnimator::PulseMode>(ledConfig.animationSettings[0]),
				NL::Pixel(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				ledConfig.animationSettings[9] / 512.0f,
				ledConfig.animationSettings[18]));
		}

		// Equalizer type
		else if (ledConfig.type == 8)
		{
			NL::LedManager::ledAnimator.at(i).reset(new NL::EqualizerAnimator(
				NL::Pixel(ledConfig.animationSettings[1], ledConfig.animationSettings[2], ledConfig.animationSettings[3]),
				NL::Pixel(ledConfig.animationSettings[4], ledConfig.animationSettings[5], ledConfig.animationSettings[6]),
				ledConfig.animationSettings[7],
				ledConfig.animationSettings[8] / 255.0f,
				ledConfig.animationSettings[18]));
		}

		// Unknown type
		else
		{
			return NL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE;
		}

		NL::LedManager::ledAnimator.at(i)->setDataSource(static_cast<NL::LedAnimator::DataSource>(ledConfig.dataSource));
		NL::LedManager::ledAnimator.at(i)->setSpeed(ledConfig.speed);
		NL::LedManager::ledAnimator.at(i)->setOffset(ledConfig.offset);
		NL::LedManager::ledAnimator.at(i)->setAnimationBrightness(ledConfig.brightness / 255.0f);
		NL::LedManager::ledAnimator.at(i)->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		NL::LedManager::ledAnimator.at(i)->setReverse(ledConfig.reverse);
		NL::LedManager::ledAnimator.at(i)->init(NL::LedManager::ledBuffer->getLedStrip(i));
	}
	return NL::LedManager::Error::OK;
}

/**
 * @brief Load a custom animator and play the animation from the fseq loader.
 * @param fileName name of the fseq file to load
 * @return OK when the custom animation was loaded
 * @return ERROR_INVALID_FSEQ when a custom animation was set but the fseq file is invalid
 * @return ERROR_INVALID_LED_CONFIGURATION when the LED configuration is invalid for the custom animation
 */
NL::LedManager::Error NL::LedManager::loadCustomAnimation(const String &fileName)
{
	NL::LedManager::fseqLoader.reset(new NL::FseqLoader(&SD));
	const NL::FseqLoader::Error fseqError = NL::LedManager::fseqLoader->loadFromFile(FSEQ_DIRECTORY + (String)F("/") + fileName);
	if (fseqError != NL::FseqLoader::Error::OK)
	{
		NL::LedManager::fseqLoader.reset();
		return NL::LedManager::Error::ERROR_INVALID_FSEQ;
	}

	const uint32_t channelCount = NL::LedManager::getLedCount() * 3;
	const uint32_t roundedChannelCount = channelCount % 4 ? channelCount + (4 - channelCount % 4) : channelCount;
	const uint8_t fillerBytes = roundedChannelCount - channelCount;
	if (NL::LedManager::fseqLoader->getHeader().channelCount != roundedChannelCount)
	{
		return NL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION;
	}
	NL::LedManager::fseqLoader->setFillerBytes(fillerBytes);
	NL::LedManager::fseqLoader->setZoneCount(LED_NUM_ZONES);

	NL::LedManager::setFrameInterval(static_cast<uint32_t>(NL::LedManager::fseqLoader->getHeader().stepTime) * 1000);

	NL::LedManager::ledAnimator.resize(LED_NUM_ZONES);
	for (size_t i = 0; i < NL::LedManager::ledAnimator.size(); i++)
	{
		NL::Configuration::LedConfig ledConfig;
		NL::Configuration::getLedConfig(i, ledConfig);

		NL::LedManager::ledAnimator.at(i).reset(new NL::FseqAnimator(NL::LedManager::fseqLoader.get(), true));
		NL::LedManager::ledAnimator.at(i)->setDataSource(static_cast<NL::LedAnimator::DataSource>(ledConfig.dataSource));
		NL::LedManager::ledAnimator.at(i)->setSpeed(ledConfig.speed);
		NL::LedManager::ledAnimator.at(i)->setOffset(ledConfig.offset);
		NL::LedManager::ledAnimator.at(i)->setAnimationBrightness(ledConfig.brightness / 255.0f);
		NL::LedManager::ledAnimator.at(i)->setFadeSpeed(ledConfig.fadeSpeed / 4096.0f);
		NL::LedManager::ledAnimator.at(i)->setReverse(ledConfig.reverse);
		NL::LedManager::ledAnimator.at(i)->init(NL::LedManager::ledBuffer->getLedStrip(i));
	}
	return NL::LedManager::Error::OK;
}

/**
 * @brief Calculate the total power draw from each regulator using the current frame.
 * @param regulatorPower array containing the power draw per regulator after the call
 */
void NL::LedManager::calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT])
{
	for (uint8_t i = 0; i < REGULATOR_COUNT; i++)
	{
		regulatorPower[i] = 0.0f;
	}

	for (size_t i = 0; i < NL::LedManager::ledBuffer->getLedStripCount(); i++)
	{
		NL::LedStrip ledStrip = NL::LedManager::ledBuffer->getLedStrip(i);
		NL::Configuration::LedConfig ledConfig;
		NL::Configuration::getLedConfig(i, ledConfig);

		float zoneCurrent = 0.0f;
		for (size_t j = 0; j < ledStrip.getLedCount(); j++)
		{
			zoneCurrent += ledConfig.ledChannelCurrent[0] * ledStrip.getPixel(j).red / 255.0f;
			zoneCurrent += ledConfig.ledChannelCurrent[1] * ledStrip.getPixel(j).green / 255.0f;
			zoneCurrent += ledConfig.ledChannelCurrent[2] * ledStrip.getPixel(j).blue / 255.0f;
		}

		const uint8_t regulatorIndex = NL::LedManager::getRegulatorIndexFromPin(ledConfig.ledPin);
		regulatorPower[regulatorIndex] += zoneCurrent * ledConfig.ledVoltage / 1000.0f;
	}
}

/**
 * @brief Limit the power consumption of the current frame to the maxumim system power.
 */
void NL::LedManager::limitPowerConsumption()
{
	float regulatorPower[REGULATOR_COUNT];
	NL::LedManager::calculateRegulatorPowerDraw(regulatorPower);

	const NL::Configuration::SystemConfig systemConfig = NL::Configuration::getSystemConfig();
	for (size_t i = 0; i < NL::LedManager::ledBuffer->getLedStripCount(); i++)
	{
		NL::LedStrip ledStrip = NL::LedManager::ledBuffer->getLedStrip(i);
		NL::Configuration::LedConfig ledConfig;
		NL::Configuration::getLedConfig(i, ledConfig);

		const uint8_t regulatorIndex = NL::LedManager::getRegulatorIndexFromPin(ledConfig.ledPin);
		float multiplicator = (static_cast<float>(systemConfig.regulatorPowerLimit) / REGULATOR_COUNT) / regulatorPower[regulatorIndex];
		if (multiplicator < 0.0f)
		{
			multiplicator = 0.0f;
		}
		else if (multiplicator > 1.0f)
		{
			multiplicator = 1.0f;
		}

		for (size_t j = 0; j < ledStrip.getLedCount(); j++)
		{
			NL::Pixel pixel = ledStrip.getPixel(j);
			pixel.red *= multiplicator;
			pixel.green *= multiplicator;
			pixel.blue *= multiplicator;
			ledStrip.setPixel(pixel, j);
		}
	}
}

/**
 * @brief Limit the regulator temperature by reducing the LED brightness once the high temperature is reached.
 */
void NL::LedManager::limitRegulatorTemperature()
{
	float multiplicator = 1.0f - (NL::LedManager::regulatorTemperature - NL::Configuration::getSystemConfig().regulatorHighTemperature) / (NL::Configuration::getSystemConfig().regulatorCutoffTemperature - NL::Configuration::getSystemConfig().regulatorHighTemperature);
	if (multiplicator < 0.0f)
	{
		multiplicator = 0.0f;
	}
	else if (multiplicator > 1.0f)
	{
		multiplicator = 1.0f;
	}

	for (size_t i = 0; i < NL::LedManager::ledBuffer->getLedStripCount(); i++)
	{
		NL::LedStrip ledStrip = NL::LedManager::ledBuffer->getLedStrip(i);
		for (size_t j = 0; j < ledStrip.getLedCount(); j++)
		{
			NL::Pixel pixel = ledStrip.getPixel(j);
			pixel.red *= multiplicator;
			pixel.green *= multiplicator;
			pixel.blue *= multiplicator;
			ledStrip.setPixel(pixel, j);
		}
	}
}

/**
 * @brief Get the regulator index by providing the pin number.
 * @param pin physical pin number
 * @return regulator index
 */
uint8_t NL::LedManager::getRegulatorIndexFromPin(const uint8_t pin)
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
