/**
 * @file LedDriver.h
 * @author TheRealKasumi
 * @brief Contains a class with an LED driver for WS2812B LEDs. It support parallel output for up to 8 channels.
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
#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdint.h>
#include <cstring>
#include <rom/ets_sys.h>
#include <rom/lldesc.h>
#include <esp_heap_caps.h>
#include <soc/i2s_reg.h>
#include <soc/i2s_struct.h>
#include <soc/io_mux_reg.h>
#include <driver/gpio.h>
#include <driver/periph_ctrl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "led/driver/LedBuffer.h"

namespace NL
{
    class LedDriver
    {
    public:
        enum class Error
        {
            OK,                       // No error
            ERROR_NO_LED_STRIPS,      // No LED strips provided
            ERROR_NOT_INITIALIZED,    // Not initialized yet
            ERROR_SET_PIN,            // Failed to configure the output pin
            ERROR_ALLOCATE_INTERRUPT, // Failed to allocate the interrupt
            ERROR_ENABLE_INTERRUPT,   // Failed to enable interrupt
            ERROR_STILL_SENDING       // When the driver is still sending LED data
        };

        enum class I2SDevice : uint8_t
        {
            I2S_DEV_0 = 0, // I2S device 0
            I2S_DEV_1 = 1  // I2S Device 1
        };

        static NL::LedDriver::Error begin(NL::LedBuffer &ledBuffer, const NL::LedDriver::I2SDevice i2sDeviceIdentifier = NL::LedDriver::I2SDevice::I2S_DEV_0);
        static bool isInitialized();
        static void end();

        static NL::LedDriver::Error isReady(const TickType_t timeout = 0);
        static NL::LedDriver::Error showPixels(const TickType_t timeout = 0);

    private:
        LedDriver();

        struct DMABuffer
        {
            lldesc_t descriptor;
            uint8_t *buffer;
        };

        static bool initialized;

        static uint8_t *ledBuffer;
        static volatile uint16_t ledIndex;
        static volatile uint16_t ledStripCount;
        static uint16_t ledStripLength[8];
        static volatile uint16_t ledStripMaxLength;

        static i2s_dev_t *i2sDevice;
        static NL::LedDriver::I2SDevice i2sDeviceIdentifier;

        static NL::LedDriver::DMABuffer *dmaBuffer[4];
        static volatile uint8_t dmaBufferIndex;

        static intr_handle_t interruptHandle;
        static volatile xSemaphoreHandle semaphore;

        static NL::LedDriver::Error initPin(const uint8_t outputPin, const uint8_t ledStripIndex);
        static NL::LedDriver::Error initI2S();
        static NL::LedDriver::Error startI2S(const NL::LedDriver::DMABuffer *startBuffer);
        static void IRAM_ATTR resetI2S();
        static void IRAM_ATTR stopI2S();
        static void resetDMA();
        static void resetFIFO();

        static NL::LedDriver::DMABuffer *allocateDMABuffer(const uint32_t size);
        static void freeDMABuffer(NL::LedDriver::DMABuffer *dmaBuffer);
        static void initDMABuffers();

        static void IRAM_ATTR interruptHandler(void *args);
        static void IRAM_ATTR loadDMABuffer(uint8_t *ledBuffer, uint16_t *dmaBuffer, const uint16_t *ledStripLength, const uint16_t ledStripCount, const uint16_t ledIndex);
        static void IRAM_ATTR transpose(uint8_t *pixelBuffer, uint16_t *dmaBuffer);
    };
};

#endif
