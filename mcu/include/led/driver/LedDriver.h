/**
 * @file LedDriver.h
 * @author TheRealKasumi
 * @brief Contains a class with an LED driver for WS2812B LEDs. It support parallel output for up to 8 channels.
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

namespace TL
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

        static TL::LedDriver::Error begin(TL::LedBuffer &ledBuffer, const TL::LedDriver::I2SDevice i2sDeviceIdentifier = TL::LedDriver::I2SDevice::I2S_DEV_0);
        static bool isInitialized();
        static void end();

        static TL::LedDriver::Error isReady(const TickType_t timeout = 0);
        static TL::LedDriver::Error showPixels(const TickType_t timeout = 0);

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
        static TL::LedDriver::I2SDevice i2sDeviceIdentifier;

        static TL::LedDriver::DMABuffer *dmaBuffer[4];
        static volatile uint8_t dmaBufferIndex;

        static intr_handle_t interruptHandle;
        static volatile xSemaphoreHandle semaphore;

        static TL::LedDriver::Error initPin(const uint8_t outputPin, const uint8_t ledStripIndex);
        static TL::LedDriver::Error initI2S();
        static TL::LedDriver::Error startI2S(const TL::LedDriver::DMABuffer *startBuffer);
        static void IRAM_ATTR resetI2S();
        static void IRAM_ATTR stopI2S();
        static void resetDMA();
        static void resetFIFO();

        static TL::LedDriver::DMABuffer *allocateDMABuffer(const uint32_t size);
        static void freeDMABuffer(TL::LedDriver::DMABuffer *dmaBuffer);
        static void initDMABuffers();

        static void IRAM_ATTR interruptHandler(void *args);
        static void IRAM_ATTR loadDMABuffer(uint8_t *ledBuffer, uint16_t *dmaBuffer, const uint16_t *ledStripLength, const uint16_t ledStripCount, const uint16_t ledIndex);
        static void IRAM_ATTR transpose(uint8_t *pixelBuffer, uint16_t *dmaBuffer);
    };
};

#endif