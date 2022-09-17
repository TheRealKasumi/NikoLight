/**
 * @file LedDriver.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::LedDriver}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led/driver/LedDriver.h"

/**
 * @brief Create a new instance of {@link TesLight::LedDriver}.
 * @param pin ESP32 pin
 * @param channel rmt lib channel to use for the output. Valid range is 0 to 7.
 * @param pixelCount number of pixels
 * @param animationBrightness brightness of the displayed LED animation from 0.0 to 1.0
 * @param lightBrightness brightness of the cars ambient light from 0.0 to 1.0
 */
TesLight::LedDriver::LedDriver(const uint8_t pin, const uint8_t channel, const uint8_t pixelCount, const float animationBrightness, const float lightBrightness, const float fadeSpeed)
{
	this->pin = pin;
	this->channel = channel;
	this->pixelCount = pixelCount;
	this->animationBrightness = animationBrightness;
	this->lightBrightness = lightBrightness;
	this->fadeSpeed = fadeSpeed;

	this->driverInstalled = false;
	this->pixels = nullptr;
	this->smoothedLightBrightness = 0.0f;
	this->totalBrightness = 0.0f;
}

/**
 * @brief Destroy the {@link TesLight::LedDriver} isntance and free it's resources.
 */
TesLight::LedDriver::~LedDriver()
{
	this->end();
}

/**
 * @brief Get the {@TesLight::Pixel} array reference.
 * @return {@TesLight::Pixel} array reference
 */
TesLight::Pixel *TesLight::LedDriver::getPixels()
{
	return this->pixels;
}

/**
 * @brief Get the number of pixels.
 * @return uint16_t number of pixels
 */
uint16_t TesLight::LedDriver::getPixelCount()
{
	return this->pixelCount;
}

/**
 * @brief Set a single {@TesLight::Pixel} by it's index.
 * @param index index of the pixe
 * @param pixel instance of {@TesLight::Pixel}
 */
void TesLight::LedDriver::setPixel(const uint16_t index, const TesLight::Pixel pixel)
{
	this->pixels[index] = pixel;
}

/**
 * @brief Get a single {@TesLight::Pixel} by it's index.
 * @param index of the {@TesLight::Pixel}
 * @return instance of {@TesLight::Pixel}
 */
TesLight::Pixel TesLight::LedDriver::getPixel(const uint16_t index)
{
	return this->pixels[index];
}

/**
 * @brief Get the animation brightness of the LED's.
 * @return brightness value between 0.0 and 1.0
 */
float TesLight::LedDriver::getAnimationBrightness()
{
	return this->animationBrightness;
}

/**
 * @brief Set the animation brightness of the LED's.
 * @param brightness brightness value between 0.0 and 1.0
 */
void TesLight::LedDriver::setAnimationBrightness(const float animationBrightness)
{
	this->animationBrightness = animationBrightness;
	if (this->animationBrightness < 0.0f)
	{
		this->animationBrightness = 0.0f;
	}
	else if (this->animationBrightness > 1.0f)
	{
		this->animationBrightness = 1.0f;
	}
}

/**
 * @brief Get the brightness of the LED's.
 * @return brightness value between 0.0 and 1.0
 */
float TesLight::LedDriver::getLightBrightness()
{
	return this->lightBrightness;
}

/**
 * @brief Set the brightness of the LED's.
 * @param brightness brightness value between 0.0 and 1.0
 */
void TesLight::LedDriver::setLightBrightness(const float lightBrightness)
{
	this->lightBrightness = lightBrightness;
	if (this->lightBrightness < 0.0f)
	{
		this->lightBrightness = 0.0f;
	}
	else if (this->lightBrightness > 1.0f)
	{
		this->lightBrightness = 1.0f;
	}
}

/**
 * @brief Get the speed for the fading when tunring on/off the LED's.
 * @return fade speed in units per frame
 */
float TesLight::LedDriver::getFadeSpeed()
{
	return this->fadeSpeed;
}

/**
 * @brief Set the speed for the fading when tunring on/off the LED's.
 * @param fadeSpeed fade speed in units per frame. Should be between 0.0 and 1.0
 */
void TesLight::LedDriver::setFadeSpeed(const float fadeSpeed)
{
	this->fadeSpeed = fadeSpeed;
	if (this->fadeSpeed < 0.0f)
	{
		this->fadeSpeed = 0.0f;
	}
	else if (this->fadeSpeed > 1.0f)
	{
		this->fadeSpeed = 1.0f;
	}
}

/**
 * @brief Initialize the LED driver and install the rmt driver.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedDriver::begin()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedDriver.cpp:begin"), (String)F("Initialize LED driver for ") + String(this->pixelCount) + F(" LEDs using pin ") + String(this->pin) + F(" and rmt channel ") + String(this->channel) + F("."));
	if (this->channel > 7)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("LedDriver.cpp:begin"), F("The channel must be between 0 and (including) 7."));
		return false;
	}

	if (this->pixelCount == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("LedDriver.cpp:begin"), F("The pixel count must not be 0."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedDriver.cpp:begin"), F("Allocating memory for pixel array."));
	this->pixels = new TesLight::Pixel[this->pixelCount];

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedDriver.cpp:begin"), F("Getting rmt driver configuration."));
	rmt_config_t rmtConfig = this->getRmtConfig();
	this->driverInstalled = this->installDriver(&rmtConfig);
	if (this->driverInstalled)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedDriver.cpp:begin"), F("LED driver initialized successfully."));
		return true;
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("LedDriver.cpp:begin"), F("Failed to initialize LED driver. The rmt driver could not be installed."));
		return false;
	}
}

/**
 * @brief Free resources and uninstall the the rmt driver.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedDriver::end()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedDriver.cpp:end"), F("Cleaning up LED driver."));
	if (this->pixels)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedDriver.cpp:end"), F("Freeing pixel array memory."));
		delete[] this->pixels;
		this->pixels = nullptr;
	}

	if (this->driverInstalled)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedDriver.cpp:end"), F("Uninstalling rmt driver."));
		rmt_config_t rmtConfig = this->getRmtConfig();
		if (!this->uninstallDriver(&rmtConfig))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("LedDriver.cpp:end"), F("Failed to uninstall rmt driver."));
			return false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedDriver.cpp:end"), F("LED driver uninstalled."));
	return true;
}

/**
 * @brief Display the {@TesLight::Pixel} array.
 * @return true when successful
 * @return false when there is an error
 */
bool TesLight::LedDriver::show()
{
	if (this->smoothedLightBrightness < this->lightBrightness)
	{
		this->smoothedLightBrightness += this->fadeSpeed;
		if (this->smoothedLightBrightness > this->lightBrightness)
		{
			this->smoothedLightBrightness = this->lightBrightness;
		}
	}
	else if (this->smoothedLightBrightness > this->lightBrightness)
	{
		this->smoothedLightBrightness -= this->fadeSpeed;
		if (this->smoothedLightBrightness < this->lightBrightness)
		{
			this->smoothedLightBrightness = this->lightBrightness;
		}
	}
	this->totalBrightness = this->animationBrightness * this->smoothedLightBrightness;

	rmt_config_t rmtConfig = this->getRmtConfig();
	rmt_item32_t *ledDataBuffer = new rmt_item32_t[this->pixelCount * BITS_PER_LED];
	this->prepareLedDataBuffer(ledDataBuffer);

	bool error = false;
	if (rmt_write_items(rmtConfig.channel, ledDataBuffer, this->pixelCount * BITS_PER_LED, false) != ESP_OK)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("LedDriver.cpp:show"), F("Failed to write led data via rmt driver."));
		error = true;
	}

	if (rmt_wait_tx_done(rmtConfig.channel, portMAX_DELAY) != ESP_OK)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("LedDriver.cpp:show"), F("Failed to wait for rmt driver to send led data."));
		error = true;
	}

	delete[] ledDataBuffer;
	return !error;
}

/**
 * @brief Get the rmt configuration.
 * @return rmt_config_t for the rmt driver
 */
rmt_config_t TesLight::LedDriver::getRmtConfig()
{
	rmt_config_t config;
	config.rmt_mode = RMT_MODE_TX;
	config.channel = (rmt_channel_t)this->channel;
	config.gpio_num = (gpio_num_t)this->pin;
	config.mem_block_num = 3;
	config.tx_config.loop_en = false;
	config.tx_config.carrier_en = false;
	config.tx_config.idle_output_en = true;
	config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
	config.clk_div = 2;
	return config;
}

/**
 * @brief Install the rmt driver.
 * @param config configuration of the driver
 * @return true when succesful
 * @return false when there was an error.
 */
bool TesLight::LedDriver::installDriver(const rmt_config_t *config)
{
	return rmt_config(config) == ESP_OK && rmt_driver_install(config->channel, 0, 0) == ESP_OK;
}

/**
 * @brief Uninstall the rmt driver.
 * @param config configuration of the driver
 * @return true when succesful
 * @return false when there was an error.
 */
bool TesLight::LedDriver::uninstallDriver(const rmt_config_t *config)
{
	return rmt_driver_uninstall(config->channel) == ESP_OK;
}

/**
 * @brief Prepare the led data buffer.
 * @param ledDataBuffer data buffer for the rmt driver
 */
void TesLight::LedDriver::prepareLedDataBuffer(rmt_item32_t *ledDataBuffer)
{
	for (uint16_t i = 0; i < this->pixelCount; i++)
	{
		TesLight::Pixel pixel = this->pixels[i];
		pixel.setRed(pixel.getRed() * this->totalBrightness);
		pixel.setGreen(pixel.getGreen() * this->totalBrightness);
		pixel.setBlue(pixel.getBlue() * this->totalBrightness);

		uint32_t mask = 1 << (BITS_PER_LED - 1);
		for (uint8_t j = 0; j < BITS_PER_LED; j++)
		{
			uint32_t bit = pixel.getColor() & mask;
			ledDataBuffer[i * BITS_PER_LED + j] = bit ? (rmt_item32_t){{{T1H, 1, TL, 0}}} : (rmt_item32_t){{{T0H, 1, TL, 0}}};
			mask >>= 1;
		}
	}
}
