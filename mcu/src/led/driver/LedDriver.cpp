#include "led/driver/LedDriver.h"

bool NL::LedDriver::initialized = false;
uint8_t *NL::LedDriver::ledBuffer;
volatile uint16_t NL::LedDriver::ledIndex;
volatile uint16_t NL::LedDriver::ledStripCount;
uint16_t NL::LedDriver::ledStripLength[8];
volatile uint16_t NL::LedDriver::ledStripMaxLength;
i2s_dev_t *NL::LedDriver::i2sDevice;
NL::LedDriver::I2SDevice NL::LedDriver::i2sDeviceIdentifier;
NL::LedDriver::DMABuffer *NL::LedDriver::dmaBuffer[4];
volatile uint8_t NL::LedDriver::dmaBufferIndex;
intr_handle_t NL::LedDriver::interruptHandle;
volatile xSemaphoreHandle NL::LedDriver::semaphore;

/**
 * @brief Initialize and start the LED driver.
 * @param ledBuffer reference to the LED buffer
 * @param i2sDevice I2S device to use
 * @return OK when the LED driver was initialized
 * @return ERROR_NO_LED_STRIPS when no LED data was provided
 * @return ERROR_SET_PIN when the pin could not be configured
 * @return ERROR_ALLOCATE_INTERRUPT when the interrupt could not be allocated
 */
NL::LedDriver::Error NL::LedDriver::begin(NL::LedBuffer &ledBuffer, const NL::LedDriver::I2SDevice i2sDeviceIdentifier)
{
    if (ledBuffer.getLedStripCount() == 0 || ledBuffer.getMaxHiddenLedCount() < 8)
    {
        return NL::LedDriver::Error::ERROR_NO_LED_STRIPS;
    }

    NL::LedDriver::initialized = false;
    NL::LedDriver::ledBuffer = nullptr;
    NL::LedDriver::ledIndex = 0;
    NL::LedDriver::ledStripCount = 0;
    std::memset(reinterpret_cast<uint8_t *>(NL::LedDriver::ledStripLength), 0, sizeof(ledStripLength));
    NL::LedDriver::ledStripMaxLength = 0;
    NL::LedDriver::i2sDevice = nullptr;
    NL::LedDriver::i2sDeviceIdentifier = NL::LedDriver::I2SDevice::I2S_DEV_0;
    std::memset(reinterpret_cast<uint8_t *>(NL::LedDriver::dmaBuffer), 0, sizeof(NL::LedDriver::dmaBuffer));
    NL::LedDriver::dmaBufferIndex = 0;
    NL::LedDriver::interruptHandle = nullptr;
    NL::LedDriver::semaphore = NULL;

    for (size_t i = 0; i < ledBuffer.getLedStripCount(); i++)
    {
        const NL::LedDriver::Error pinError = NL::LedDriver::initPin(ledBuffer.getLedStrip(i).getLedPin(), i);
        if (pinError != NL::LedDriver::Error::OK)
        {
            return pinError;
        }
        NL::LedDriver::ledStripLength[i] = ledBuffer.getLedStrip(i).getHiddenLedCount();
    }

    NL::LedDriver::ledBuffer = ledBuffer.getBuffer();
    NL::LedDriver::ledStripCount = ledBuffer.getLedStripCount();
    NL::LedDriver::ledStripMaxLength = ledBuffer.getMaxHiddenLedCount();
    NL::LedDriver::i2sDeviceIdentifier = i2sDeviceIdentifier;
    NL::LedDriver::i2sDeviceIdentifier = i2sDeviceIdentifier;
    NL::LedDriver::semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(NL::LedDriver::semaphore);

    const NL::LedDriver::Error i2sError = NL::LedDriver::initI2S();
    if (i2sError != NL::LedDriver::Error::OK)
    {
        return i2sError;
    }
    NL::LedDriver::initDMABuffers();

    NL::LedDriver::initialized = true;
    return NL::LedDriver::Error::OK;
}

/**
 * @brief Check if the LED driver was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::LedDriver::isInitialized()
{
    return NL::LedDriver::initialized;
}

/**
 * @brief Stop the LED driver and free resources.
 */
void NL::LedDriver::end()
{
    if (NL::LedDriver::initialized)
    {
        xSemaphoreTake(NL::LedDriver::semaphore, portMAX_DELAY);
        xSemaphoreGive(NL::LedDriver::semaphore);
        vSemaphoreDelete(semaphore);
        esp_intr_free(interruptHandle);

        NL::LedDriver::initialized = false;
        NL::LedDriver::freeDMABuffer(dmaBuffer[0]);
        NL::LedDriver::freeDMABuffer(dmaBuffer[1]);
        NL::LedDriver::freeDMABuffer(dmaBuffer[2]);
        NL::LedDriver::freeDMABuffer(dmaBuffer[3]);
    }
}

/**
 * @brief Check if the driver finished sending all data and is ready for new one.
 * @param timeout cpu cycles until a timeout will occur when the output is not finished yet
 * @return OK when the LED driver is ready
 * @return ERROR_NOT_INITIALIZED when the LED driver was not properly initialized yet
 * @return ERROR_STILL_SENDING when the output is still in progress and the driver is not ready yet
 */
NL::LedDriver::Error NL::LedDriver::isReady(const TickType_t timeout)
{
    if (!NL::LedDriver::initialized)
    {
        return NL::LedDriver::Error::ERROR_NOT_INITIALIZED;
    }

    if (xSemaphoreTake(NL::LedDriver::semaphore, timeout) != pdTRUE)
    {
        return NL::LedDriver::Error::ERROR_STILL_SENDING;
    }

    xSemaphoreGive(NL::LedDriver::semaphore);
    return NL::LedDriver::Error::OK;
}

/**
 * @brief Prepare the DMA buffers and send the data to the LED strips (async).
 * @param timeout cpu cycles until a timeout will occur when the output is not finished yet
 * @return OK when the LED data is being processed
 * @return ERROR_NOT_INITIALIZED when the LED driver was not properly initialized yet
 * @return ERROR_STILL_SENDING when the output is still in progress and the driver is not ready yet
 * @return ERROR_ENABLE_INTERRUPT when the interrupt could not be enabled
 */
NL::LedDriver::Error NL::LedDriver::showPixels(const TickType_t timeout)
{
    if (!NL::LedDriver::initialized)
    {
        return NL::LedDriver::Error::ERROR_NOT_INITIALIZED;
    }

    if (xSemaphoreTake(NL::LedDriver::semaphore, timeout) != pdTRUE)
    {
        return NL::LedDriver::Error::ERROR_STILL_SENDING;
    }

    NL::LedDriver::ledIndex = 0;
    NL::LedDriver::dmaBufferIndex = 1;
    NL::LedDriver::dmaBuffer[0]->descriptor.qe.stqe_next = &(dmaBuffer[1]->descriptor);
    NL::LedDriver::dmaBuffer[1]->descriptor.qe.stqe_next = &(dmaBuffer[0]->descriptor);
    NL::LedDriver::dmaBuffer[2]->descriptor.qe.stqe_next = &(dmaBuffer[0]->descriptor);
    NL::LedDriver::dmaBuffer[3]->descriptor.qe.stqe_next = 0;
    NL::LedDriver::loadDMABuffer(NL::LedDriver::ledBuffer, reinterpret_cast<uint16_t *>(NL::LedDriver::dmaBuffer[0]->buffer), NL::LedDriver::ledStripLength, NL::LedDriver::ledStripCount, NL::LedDriver::ledIndex);

    NL::LedDriver::Error startError = NL::LedDriver::startI2S(dmaBuffer[2]);
    if (startError != NL::LedDriver::Error::OK)
    {
        return startError;
    }

    return NL::LedDriver::Error::OK;
}

/**
 * @brief Initialize an output pin.
 * @param outputPin pin number of the output pin
 * @param ledStripIndex index of the assigned LED strip
 * @return OK when the pin was configured
 * @return ERROR_SET_PIN when the pin could not be configured
 */
NL::LedDriver::Error NL::LedDriver::initPin(const uint8_t outputPin, const uint8_t ledStripIndex)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[outputPin], PIN_FUNC_GPIO);
    if (gpio_set_direction(static_cast<gpio_num_t>(outputPin), (gpio_mode_t)GPIO_MODE_DEF_OUTPUT) != ESP_OK)
    {
        return NL::LedDriver::Error::ERROR_SET_PIN;
    }
    gpio_matrix_out(outputPin, (NL::LedDriver::i2sDeviceIdentifier == NL::LedDriver::I2SDevice::I2S_DEV_0 ? I2S0O_DATA_OUT0_IDX : I2S1O_DATA_OUT0_IDX) + ledStripIndex + 8, false, false);
    return NL::LedDriver::Error::OK;
}

/**
 * @brief Initialize the I2S device and prepare DMA buffers.
 * @return OK when the I2S device was initialized
 * @return ERROR_ALLOCATE_INTERRUPT when the interrupt could not be allocated
 */
NL::LedDriver::Error NL::LedDriver::initI2S()
{
    uint8_t interruptSource;
    if (NL::LedDriver::i2sDeviceIdentifier == NL::LedDriver::I2SDevice::I2S_DEV_0)
    {
        NL::LedDriver::i2sDevice = &I2S0;
        periph_module_enable(PERIPH_I2S0_MODULE);
        interruptSource = ETS_I2S0_INTR_SOURCE;
    }
    else
    {
        NL::LedDriver::i2sDevice = &I2S1;
        periph_module_enable(PERIPH_I2S1_MODULE);
        interruptSource = ETS_I2S1_INTR_SOURCE;
    }

    NL::LedDriver::resetI2S();
    NL::LedDriver::resetDMA();
    NL::LedDriver::resetFIFO();

    NL::LedDriver::i2sDevice->conf.tx_right_first = 0;
    NL::LedDriver::i2sDevice->conf2.val = 0;
    NL::LedDriver::i2sDevice->conf2.lcd_en = 1;
    NL::LedDriver::i2sDevice->conf2.lcd_tx_wrx2_en = 1;
    NL::LedDriver::i2sDevice->conf2.lcd_tx_sdx2_en = 0;
    NL::LedDriver::i2sDevice->sample_rate_conf.val = 0;
    NL::LedDriver::i2sDevice->sample_rate_conf.tx_bits_mod = 16;
    NL::LedDriver::i2sDevice->clkm_conf.val = 0;
    NL::LedDriver::i2sDevice->clkm_conf.clka_en = 0;
    NL::LedDriver::i2sDevice->clkm_conf.clkm_div_a = 3;
    NL::LedDriver::i2sDevice->clkm_conf.clkm_div_b = 1;
    NL::LedDriver::i2sDevice->clkm_conf.clkm_div_num = 33;
    NL::LedDriver::i2sDevice->fifo_conf.val = 0;
    NL::LedDriver::i2sDevice->fifo_conf.tx_fifo_mod_force_en = 1;
    NL::LedDriver::i2sDevice->fifo_conf.tx_fifo_mod = 1;
    NL::LedDriver::i2sDevice->fifo_conf.tx_data_num = 32;
    NL::LedDriver::i2sDevice->fifo_conf.dscr_en = 1;
    NL::LedDriver::i2sDevice->sample_rate_conf.tx_bck_div_num = 1;
    NL::LedDriver::i2sDevice->conf1.val = 0;
    NL::LedDriver::i2sDevice->conf1.tx_stop_en = 0;
    NL::LedDriver::i2sDevice->conf1.tx_pcm_bypass = 1;
    NL::LedDriver::i2sDevice->conf_chan.val = 0;
    NL::LedDriver::i2sDevice->conf_chan.tx_chan_mod = 1;
    NL::LedDriver::i2sDevice->timing.val = 0;
    NL::LedDriver::i2sDevice->int_ena.val = 0;

    if (esp_intr_alloc(interruptSource, ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM, &NL::LedDriver::interruptHandler, NULL, &NL::LedDriver::interruptHandle) != ESP_OK)
    {
        return NL::LedDriver::Error::ERROR_ALLOCATE_INTERRUPT;
    }

    return NL::LedDriver::Error::OK;
}

/**
 * @brief Start the I2S device to send out the LED data.
 * @param startBuffer first buffer to read the data from
 * @return OK when the I2S device started to send out the LED data
 * @return ERROR_ENABLE_INTERRUPT when the interrupt could not be enabled
 */
NL::LedDriver::Error NL::LedDriver::startI2S(const NL::LedDriver::DMABuffer *startBuffer)
{
    NL::LedDriver::resetI2S();

    NL::LedDriver::i2sDevice->lc_conf.val = I2S_OUT_DATA_BURST_EN | I2S_OUTDSCR_BURST_EN | I2S_OUT_DATA_BURST_EN;
    NL::LedDriver::i2sDevice->out_link.addr = reinterpret_cast<uint32_t>(&(startBuffer->descriptor));
    NL::LedDriver::i2sDevice->out_link.start = 1;
    NL::LedDriver::i2sDevice->int_clr.val = NL::LedDriver::i2sDevice->int_raw.val;
    NL::LedDriver::i2sDevice->int_clr.val = NL::LedDriver::i2sDevice->int_raw.val;
    NL::LedDriver::i2sDevice->int_ena.val = 0;
    NL::LedDriver::i2sDevice->int_ena.out_eof = 1;
    NL::LedDriver::i2sDevice->int_ena.out_total_eof = 1;

    if (esp_intr_enable(NL::LedDriver::interruptHandle) != ESP_OK)
    {
        return NL::LedDriver::Error::ERROR_ENABLE_INTERRUPT;
    }

    NL::LedDriver::i2sDevice->conf.tx_start = 1;
    return NL::LedDriver::Error::OK;
}

/**
 * @brief Reset the I2S device.
 */
void IRAM_ATTR NL::LedDriver::resetI2S()
{
    const unsigned long lcConfigResetFlage = I2S_IN_RST_M | I2S_OUT_RST_M | I2S_AHBM_RST_M | I2S_AHBM_FIFO_RST_M;
    NL::LedDriver::i2sDevice->lc_conf.val |= lcConfigResetFlage;
    NL::LedDriver::i2sDevice->lc_conf.val &= ~lcConfigResetFlage;
    const uint32_t configResetFlags = I2S_RX_RESET_M | I2S_RX_FIFO_RESET_M | I2S_TX_RESET_M | I2S_TX_FIFO_RESET_M;
    NL::LedDriver::i2sDevice->conf.val |= configResetFlags;
    NL::LedDriver::i2sDevice->conf.val &= ~configResetFlags;
}

/**
 * @brief Stop the I2S device after all data was sent to the LEDs.
 */
void IRAM_ATTR NL::LedDriver::stopI2S()
{
    if (esp_intr_disable(NL::LedDriver::interruptHandle) != ESP_OK)
    {
        // Idk...
    }

    NL::LedDriver::i2sDevice->conf.tx_start = 0;
    // while (NL::LedDriver::i2sDevice->conf.tx_start == 1);
    resetI2S();
    ets_delay_us(30);
}

/**
 * @brief Reset the DMA of the I2S device.
 */
void NL::LedDriver::resetDMA()
{
    NL::LedDriver::i2sDevice->lc_conf.out_rst = 1;
    NL::LedDriver::i2sDevice->lc_conf.out_rst = 0;
}

/**
 * @brief Reset the FIFO of the I2S device.
 */
void NL::LedDriver::resetFIFO()
{
    NL::LedDriver::i2sDevice->conf.tx_fifo_reset = 1;
    NL::LedDriver::i2sDevice->conf.tx_fifo_reset = 0;
}

/**
 * @brief Allocate a DMA buffer with a given size.
 * @param size number of elements to allocate
 * @return allocated DMA buffer
 */
NL::LedDriver::DMABuffer *NL::LedDriver::allocateDMABuffer(const uint32_t size)
{
    DMABuffer *dmaBuffer = reinterpret_cast<DMABuffer *>(heap_caps_malloc(sizeof(DMABuffer), MALLOC_CAP_DMA));
    dmaBuffer->buffer = reinterpret_cast<uint8_t *>(heap_caps_malloc(size, MALLOC_CAP_DMA));

    std::memset(dmaBuffer->buffer, 0, size);
    dmaBuffer->descriptor.length = size;
    dmaBuffer->descriptor.size = size;
    dmaBuffer->descriptor.owner = 1;
    dmaBuffer->descriptor.sosf = 1;
    dmaBuffer->descriptor.buf = dmaBuffer->buffer;
    dmaBuffer->descriptor.offset = 0;
    dmaBuffer->descriptor.empty = 0;
    dmaBuffer->descriptor.eof = 1;
    dmaBuffer->descriptor.qe.stqe_next = 0;
    return dmaBuffer;
}

/**
 * @brief Free a given DMA buffer.
 * @param dmaBuffer DMA buffer to free
 */
void NL::LedDriver::freeDMABuffer(NL::LedDriver::DMABuffer *dmaBuffer)
{
    heap_caps_free(dmaBuffer->buffer);
    heap_caps_free(dmaBuffer);
}

/**
 * @brief Initialize the DMA buffers.
 */
void NL::LedDriver::initDMABuffers()
{
    NL::LedDriver::dmaBuffer[0] = NL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3);
    NL::LedDriver::dmaBuffer[1] = NL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3);
    NL::LedDriver::dmaBuffer[2] = NL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3);
    NL::LedDriver::dmaBuffer[3] = NL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3 * 4);

    for (uint8_t i = 0; i < 2; i++)
    {
        for (uint8_t j = 0; j < 3 * 8 / 2; j++)
        {
            uint16_t *buffer = reinterpret_cast<uint16_t *>(NL::LedDriver::dmaBuffer[i]->buffer);
            buffer[j * 6 + 1] = 0xffff;
            buffer[j * 6 + 2] = 0xffff;
        }
    }
}

/**
 * @brief Interrupt handler is called once a buffer was sent. It will then preload the next buffer.
 * @param args arguments, currently not used
 */
void IRAM_ATTR NL::LedDriver::interruptHandler(void *args)
{
    if (GET_PERI_REG_BITS(I2S_INT_ST_REG(static_cast<uint8_t>(NL::LedDriver::i2sDeviceIdentifier)), I2S_OUT_EOF_INT_ST_V, I2S_OUT_EOF_INT_ST_S))
    {
        NL::LedDriver::ledIndex++;
        if (NL::LedDriver::ledIndex < NL::LedDriver::ledStripMaxLength)
        {
            loadDMABuffer(NL::LedDriver::ledBuffer, reinterpret_cast<uint16_t *>(NL::LedDriver::dmaBuffer[NL::LedDriver::dmaBufferIndex]->buffer), NL::LedDriver::ledStripLength, NL::LedDriver::ledStripCount, NL::LedDriver::ledIndex);

            if (NL::LedDriver::ledIndex == NL::LedDriver::ledStripMaxLength - 3)
            {
                NL::LedDriver::dmaBuffer[NL::LedDriver::dmaBufferIndex]->descriptor.qe.stqe_next = &(NL::LedDriver::dmaBuffer[3]->descriptor);
            }
            NL::LedDriver::dmaBufferIndex = (NL::LedDriver::dmaBufferIndex + 1) % 2;
        }
    }

    if (GET_PERI_REG_BITS(I2S_INT_ST_REG(static_cast<uint8_t>(NL::LedDriver::i2sDeviceIdentifier)), I2S_OUT_TOTAL_EOF_INT_ST_V, I2S_OUT_TOTAL_EOF_INT_ST_S))
    {
        NL::LedDriver::stopI2S();
        portBASE_TYPE hpTaskAwoken = pdFALSE;
        xSemaphoreGiveFromISR(NL::LedDriver::semaphore, &hpTaskAwoken);
        if (hpTaskAwoken == pdTRUE)
        {
            portYIELD_FROM_ISR(hpTaskAwoken);
        }
    }
    REG_WRITE(I2S_INT_CLR_REG(0), (REG_READ(I2S_INT_RAW_REG(0)) & 0xffffffc0) | 0x3f);
}

/**
 * @brief Preload a DMA buffer from the LED pixel data.
 * @param ledBuffer led buffer with the pixel data
 * @param dmaBuffer DMA buffer to fill
 * @param ledStripLength length of the individual LED strips
 * @param ledStripCount number of LED strips
 * @param ledIndex the current LED index
 */
void IRAM_ATTR NL::LedDriver::loadDMABuffer(uint8_t *ledBuffer, uint16_t *dmaBuffer, const uint16_t *ledStripLength, const uint16_t ledStripCount, const uint16_t ledIndex)
{
    uint8_t bytes[3][16] = {{0}};
    uint8_t *poli = ledBuffer + ledIndex * 3;
    for (int i = 0; i < ledStripCount; i++)
    {
        if (ledIndex < ledStripLength[i])
        {
            bytes[0][i] = *(poli + 1);
            bytes[1][i] = *(poli + 0);
            bytes[2][i] = *(poli + 2);
        }

        poli += ledStripLength[i] * 3;
    }

    transpose(bytes[0], reinterpret_cast<uint16_t *>(dmaBuffer));
    transpose(bytes[1], reinterpret_cast<uint16_t *>(dmaBuffer + 3 * 8));
    transpose(bytes[2], reinterpret_cast<uint16_t *>(dmaBuffer + 2 * 3 * 8));
}

/**
 * @brief Transpose the LED data into the DMA buffers.
 * Todo: clean and document this mess
 */
void IRAM_ATTR NL::LedDriver::transpose(uint8_t *pixelBuffer, uint16_t *dmaBuffer)
{
    uint32_t x, y, x1, y1, t;
    y = *reinterpret_cast<unsigned int *>(pixelBuffer);
    x = *reinterpret_cast<unsigned int *>(pixelBuffer + 4);
    t = (x ^ (x >> 7)) & 0x00AA00AAL;
    x = x ^ t ^ (t << 7);
    t = (x ^ (x >> 14)) & 0x0000CCCCL;
    x = x ^ t ^ (t << 14);
    t = (y ^ (y >> 7)) & 0x00AA00AAL;
    y = y ^ t ^ (t << 7);
    t = (y ^ (y >> 14)) & 0x0000CCCCL;
    y = y ^ t ^ (t << 14);
    t = (x & 0xF0F0F0F0L) | ((y >> 4) & 0x0F0F0F0FL);
    y = ((x << 4) & 0xF0F0F0F0L) | (y & 0x0F0F0F0FL);
    x = t;
    t = (x1 & 0xF0F0F0F0L) | ((y1 >> 4) & 0x0F0F0F0FL);
    y1 = ((x1 << 4) & 0xF0F0F0F0L) | (y1 & 0x0F0F0F0FL);
    x1 = t;
    *(reinterpret_cast<uint16_t *>(dmaBuffer)) = static_cast<uint16_t>(((x & 0xff000000) >> 8 | ((x1 & 0xff000000))) >> 16);
    *(reinterpret_cast<uint16_t *>(dmaBuffer + 5)) = static_cast<uint16_t>(((x & 0xff0000) >> 16 | ((x1 & 0xff0000) >> 8)));
    *(reinterpret_cast<uint16_t *>(dmaBuffer + 6)) = static_cast<uint16_t>(((x & 0xff00) | ((x1 & 0xff00) << 8)) >> 8);
    *(reinterpret_cast<uint16_t *>(dmaBuffer + 11)) = static_cast<uint16_t>((x & 0xff) | ((x1 & 0xff) << 8));
    *(reinterpret_cast<uint16_t *>(dmaBuffer + 12)) = static_cast<uint16_t>(((y & 0xff000000) >> 8 | ((y1 & 0xff000000))) >> 16);
    *(reinterpret_cast<uint16_t *>(dmaBuffer + 17)) = static_cast<uint16_t>(((y & 0xff0000) | ((y1 & 0xff0000) << 8)) >> 16);
    *(reinterpret_cast<uint16_t *>(dmaBuffer + 18)) = static_cast<uint16_t>(((y & 0xff00) | ((y1 & 0xff00) << 8)) >> 8);
    *(reinterpret_cast<uint16_t *>(dmaBuffer + 23)) = static_cast<uint16_t>((y & 0xff) | ((y1 & 0xff) << 8));
}
