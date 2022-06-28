/**
 * @file LedDriver.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::LedDriver}.
 * @version 0.0.1
 * @date 2022-06-28
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
 * @param logger instance of {@link TesLight::Logger}
 */
TesLight::LedDriver::LedDriver(const uint8_t pin, const uint8_t channel, const uint8_t pixelCount, TesLight::Logger *logger)
{
	this->pin = pin;
	this->channel = channel;
	this->driverInstalled = false;
	this->pixelCount = pixelCount;
	this->pixels = nullptr;
	this->logger = logger;
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
 * @brief Initialize the LED driver and install the rmt driver.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedDriver::begin()
{
	if (this->channel > 7)
	{
		logger->error(F("LedDriver.cpp:begin"), F("The channel must be in the valid range of 0 to 7."));
		return false;
	}

	if (this->pixelCount == 0)
	{
		logger->error(F("LedDriver.cpp:begin"), F("The pixel count must not be 0."));
		return false;
	}

	this->pixels = new TesLight::Pixel[this->pixelCount];

	rmt_config_t rmtConfig = this->getRmtConfig();
	this->driverInstalled = this->installDriver(&rmtConfig);
	if (!this->driverInstalled)
	{
		logger->error(F("LedDriver.cpp:begin"), F("Failed to install rmt driver for data transmission to leds."));
		return false;
	}

	return true;
}

/**
 * @brief Free resources and uninstall the the rmt driver.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::LedDriver::end()
{
	if (this->pixels)
	{
		delete[] this->pixels;
		this->pixels = nullptr;
	}

	if (this->driverInstalled)
	{
		rmt_config_t rmtConfig = this->getRmtConfig();
		if (!this->uninstallDriver(&rmtConfig))
		{
			logger->error(F("LedDriver.cpp:end"), F("Failed to uninstall rmt driver."));
			return false;
		}
	}

	return true;
}

/**
 * @brief Display the {@TesLight::Pixel} array.
 */
void TesLight::LedDriver::show()
{
	rmt_config_t rmtConfig = this->getRmtConfig();
	rmt_item32_t *ledDataBuffer = new rmt_item32_t[this->pixelCount * BITS_PER_LED_CMD];
	this->prepareLedDataBuffer(ledDataBuffer);

	if (rmt_write_items(rmtConfig.channel, ledDataBuffer, this->pixelCount * BITS_PER_LED_CMD, false) != ESP_OK)
	{
		logger->error(F("LedDriver.cpp:installDriver"), F("Failed to write led data via rmt driver."));
	}

	if (rmt_wait_tx_done(rmtConfig.channel, portMAX_DELAY) != ESP_OK)
	{
		logger->error(F("LedDriver.cpp:installDriver"), F("Failed to wait for rmt driver to send led data."));
	}

	delete[] ledDataBuffer;
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
		uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);
		for (uint8_t j = 0; j < BITS_PER_LED_CMD; j++)
		{
			uint32_t bit_is_set = this->pixels[i].getColor() & mask;
			ledDataBuffer[i * BITS_PER_LED_CMD + j] = bit_is_set ? (rmt_item32_t){{{T1H, 1, TL, 0}}} : (rmt_item32_t){{{T0H, 1, TL, 0}}};
			mask >>= 1;
		}
	}
}
