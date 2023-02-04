#include "led/driver/LedDriver.h"

bool TL::LedDriver::initialized = false;
uint8_t *TL::LedDriver::ledBuffer;
volatile uint16_t TL::LedDriver::ledIndex;
volatile uint16_t TL::LedDriver::ledStripCount;
uint16_t TL::LedDriver::ledStripLength[8];
volatile uint16_t TL::LedDriver::ledStripMaxLength;
i2s_dev_t *TL::LedDriver::i2sDevice;
TL::LedDriver::I2SDevice TL::LedDriver::i2sDeviceIdentifier;
TL::LedDriver::DMABuffer *TL::LedDriver::dmaBuffer[4];
volatile uint8_t TL::LedDriver::dmaBufferIndex;
intr_handle_t TL::LedDriver::interruptHandle;
volatile xSemaphoreHandle TL::LedDriver::semaphore;

/**
 * @brief Initialize and start the LED driver.
 * @param ledBuffer reference to the LED buffer
 * @param i2sDevice I2S device to use
 * @return OK when the LED driver was initialized
 * @return ERROR_NO_LED_STRIPS when no LED data was provided
 * @return ERROR_SET_PIN when the pin could not be configured
 * @return ERROR_ALLOCATE_INTERRUPT when the interrupt could not be allocated
 */
TL::LedDriver::Error TL::LedDriver::begin(TL::LedBuffer &ledBuffer, const TL::LedDriver::I2SDevice i2sDeviceIdentifier)
{
    if (ledBuffer.getLedStripCount() == 0 || ledBuffer.getMaxHiddenLedCount() < 8)
    {
        return TL::LedDriver::Error::ERROR_NO_LED_STRIPS;
    }

    TL::LedDriver::initialized = false;
    TL::LedDriver::ledBuffer = nullptr;
    TL::LedDriver::ledIndex = 0;
    TL::LedDriver::ledStripCount = 0;
    std::memset(reinterpret_cast<uint8_t *>(TL::LedDriver::ledStripLength), 0, sizeof(ledStripLength));
    TL::LedDriver::ledStripMaxLength = 0;
    TL::LedDriver::i2sDevice = nullptr;
    TL::LedDriver::i2sDeviceIdentifier = TL::LedDriver::I2SDevice::I2S_DEV_0;
    std::memset(reinterpret_cast<uint8_t *>(TL::LedDriver::dmaBuffer), 0, sizeof(TL::LedDriver::dmaBuffer));
    TL::LedDriver::dmaBufferIndex = 0;
    TL::LedDriver::interruptHandle = nullptr;
    TL::LedDriver::semaphore = NULL;

    for (size_t i = 0; i < ledBuffer.getLedStripCount(); i++)
    {
        const TL::LedDriver::Error pinError = TL::LedDriver::initPin(ledBuffer.getLedStrip(i).getLedPin(), i);
        if (pinError != TL::LedDriver::Error::OK)
        {
            return pinError;
        }
        TL::LedDriver::ledStripLength[i] = ledBuffer.getLedStrip(i).getHiddenLedCount();
    }

    TL::LedDriver::ledBuffer = ledBuffer.getBuffer();
    TL::LedDriver::ledStripCount = ledBuffer.getLedStripCount();
    TL::LedDriver::ledStripMaxLength = ledBuffer.getMaxHiddenLedCount();
    TL::LedDriver::i2sDeviceIdentifier = i2sDeviceIdentifier;
    TL::LedDriver::i2sDeviceIdentifier = i2sDeviceIdentifier;
    TL::LedDriver::semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(TL::LedDriver::semaphore);

    const TL::LedDriver::Error i2sError = TL::LedDriver::initI2S();
    if (i2sError != TL::LedDriver::Error::OK)
    {
        return i2sError;
    }
    TL::LedDriver::initDMABuffers();

    TL::LedDriver::initialized = true;
    return TL::LedDriver::Error::OK;
}

/**
 * @brief Return if the LED driver was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::LedDriver::isInitialized()
{
    return TL::LedDriver::initialized;
}

/**
 * @brief Stop the LED driver and free resources.
 */
void TL::LedDriver::end()
{
    if (TL::LedDriver::initialized)
    {
        xSemaphoreTake(TL::LedDriver::semaphore, portMAX_DELAY);
        xSemaphoreGive(TL::LedDriver::semaphore);
        vSemaphoreDelete(semaphore);
        esp_intr_free(interruptHandle);

        TL::LedDriver::initialized = false;
        TL::LedDriver::freeDMABuffer(dmaBuffer[0]);
        TL::LedDriver::freeDMABuffer(dmaBuffer[1]);
        TL::LedDriver::freeDMABuffer(dmaBuffer[2]);
        TL::LedDriver::freeDMABuffer(dmaBuffer[3]);
    }
}

/**
 * @brief Check if the driver finished sending all data and is ready for new one.
 * @param timeout cpu cycles until a timeout will occur when the output is not finished yet
 * @return OK when the LED driver is ready
 * @return ERROR_NOT_INITIALIZED when the LED driver was not properly initialized yet
 * @return ERROR_STILL_SENDING when the output is still in progress and the driver is not ready yet
 */
TL::LedDriver::Error TL::LedDriver::isReady(const TickType_t timeout)
{
    if (!TL::LedDriver::initialized)
    {
        return TL::LedDriver::Error::ERROR_NOT_INITIALIZED;
    }

    if (xSemaphoreTake(TL::LedDriver::semaphore, timeout) != pdTRUE)
    {
        return TL::LedDriver::Error::ERROR_STILL_SENDING;
    }

    xSemaphoreGive(TL::LedDriver::semaphore);
    return TL::LedDriver::Error::OK;
}

/**
 * @brief Prepare the DMA buffers and send the data to the LED strips (async).
 * @param timeout cpu cycles until a timeout will occur when the output is not finished yet
 * @return OK when the LED data is being processed
 * @return ERROR_NOT_INITIALIZED when the LED driver was not properly initialized yet
 * @return ERROR_STILL_SENDING when the output is still in progress and the driver is not ready yet
 * @return ERROR_ENABLE_INTERRUPT when the interrupt could not be enabled
 */
TL::LedDriver::Error TL::LedDriver::showPixels(const TickType_t timeout)
{
    if (!TL::LedDriver::initialized)
    {
        return TL::LedDriver::Error::ERROR_NOT_INITIALIZED;
    }

    if (xSemaphoreTake(TL::LedDriver::semaphore, timeout) != pdTRUE)
    {
        return TL::LedDriver::Error::ERROR_STILL_SENDING;
    }

    TL::LedDriver::ledIndex = 0;
    TL::LedDriver::dmaBufferIndex = 1;
    TL::LedDriver::dmaBuffer[0]->descriptor.qe.stqe_next = &(dmaBuffer[1]->descriptor);
    TL::LedDriver::dmaBuffer[1]->descriptor.qe.stqe_next = &(dmaBuffer[0]->descriptor);
    TL::LedDriver::dmaBuffer[2]->descriptor.qe.stqe_next = &(dmaBuffer[0]->descriptor);
    TL::LedDriver::dmaBuffer[3]->descriptor.qe.stqe_next = 0;
    TL::LedDriver::loadDMABuffer(TL::LedDriver::ledBuffer, reinterpret_cast<uint16_t *>(TL::LedDriver::dmaBuffer[0]->buffer), TL::LedDriver::ledStripLength, TL::LedDriver::ledStripCount, TL::LedDriver::ledIndex);

    TL::LedDriver::Error startError = TL::LedDriver::startI2S(dmaBuffer[2]);
    if (startError != TL::LedDriver::Error::OK)
    {
        return startError;
    }

    return TL::LedDriver::Error::OK;
}

/**
 * @brief Initialize an output pin.
 * @param outputPin pin number of the output pin
 * @param ledStripIndex index of the assigned LED strip
 * @return OK when the pin was configured
 * @return ERROR_SET_PIN when the pin could not be configured
 */
TL::LedDriver::Error TL::LedDriver::initPin(const uint8_t outputPin, const uint8_t ledStripIndex)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[outputPin], PIN_FUNC_GPIO);
    if (gpio_set_direction(static_cast<gpio_num_t>(outputPin), (gpio_mode_t)GPIO_MODE_DEF_OUTPUT) != ESP_OK)
    {
        return TL::LedDriver::Error::ERROR_SET_PIN;
    }
    gpio_matrix_out(outputPin, (TL::LedDriver::i2sDeviceIdentifier == TL::LedDriver::I2SDevice::I2S_DEV_0 ? I2S0O_DATA_OUT0_IDX : I2S1O_DATA_OUT0_IDX) + ledStripIndex + 8, false, false);
    return TL::LedDriver::Error::OK;
}

/**
 * @brief Initialize the I2S device and prepare DMA buffers.
 * @return OK when the I2S device was initialized
 * @return ERROR_ALLOCATE_INTERRUPT when the interrupt could not be allocated
 */
TL::LedDriver::Error TL::LedDriver::initI2S()
{
    uint8_t interruptSource;
    if (TL::LedDriver::i2sDeviceIdentifier == TL::LedDriver::I2SDevice::I2S_DEV_0)
    {
        TL::LedDriver::i2sDevice = &I2S0;
        periph_module_enable(PERIPH_I2S0_MODULE);
        interruptSource = ETS_I2S0_INTR_SOURCE;
    }
    else
    {
        TL::LedDriver::i2sDevice = &I2S1;
        periph_module_enable(PERIPH_I2S1_MODULE);
        interruptSource = ETS_I2S1_INTR_SOURCE;
    }

    TL::LedDriver::resetI2S();
    TL::LedDriver::resetDMA();
    TL::LedDriver::resetFIFO();

    TL::LedDriver::i2sDevice->conf.tx_right_first = 0;
    TL::LedDriver::i2sDevice->conf2.val = 0;
    TL::LedDriver::i2sDevice->conf2.lcd_en = 1;
    TL::LedDriver::i2sDevice->conf2.lcd_tx_wrx2_en = 1;
    TL::LedDriver::i2sDevice->conf2.lcd_tx_sdx2_en = 0;
    TL::LedDriver::i2sDevice->sample_rate_conf.val = 0;
    TL::LedDriver::i2sDevice->sample_rate_conf.tx_bits_mod = 16;
    TL::LedDriver::i2sDevice->clkm_conf.val = 0;
    TL::LedDriver::i2sDevice->clkm_conf.clka_en = 0;
    TL::LedDriver::i2sDevice->clkm_conf.clkm_div_a = 3;
    TL::LedDriver::i2sDevice->clkm_conf.clkm_div_b = 1;
    TL::LedDriver::i2sDevice->clkm_conf.clkm_div_num = 33;
    TL::LedDriver::i2sDevice->fifo_conf.val = 0;
    TL::LedDriver::i2sDevice->fifo_conf.tx_fifo_mod_force_en = 1;
    TL::LedDriver::i2sDevice->fifo_conf.tx_fifo_mod = 1;
    TL::LedDriver::i2sDevice->fifo_conf.tx_data_num = 32;
    TL::LedDriver::i2sDevice->fifo_conf.dscr_en = 1;
    TL::LedDriver::i2sDevice->sample_rate_conf.tx_bck_div_num = 1;
    TL::LedDriver::i2sDevice->conf1.val = 0;
    TL::LedDriver::i2sDevice->conf1.tx_stop_en = 0;
    TL::LedDriver::i2sDevice->conf1.tx_pcm_bypass = 1;
    TL::LedDriver::i2sDevice->conf_chan.val = 0;
    TL::LedDriver::i2sDevice->conf_chan.tx_chan_mod = 1;
    TL::LedDriver::i2sDevice->timing.val = 0;
    TL::LedDriver::i2sDevice->int_ena.val = 0;

    if (esp_intr_alloc(interruptSource, ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM, &TL::LedDriver::interruptHandler, NULL, &TL::LedDriver::interruptHandle) != ESP_OK)
    {
        return TL::LedDriver::Error::ERROR_ALLOCATE_INTERRUPT;
    }

    return TL::LedDriver::Error::OK;
}

/**
 * @brief Start the I2S device to send out the LED data.
 * @param startBuffer first buffer to read the data from
 * @return OK when the I2S device started to send out the LED data
 * @return ERROR_ENABLE_INTERRUPT when the interrupt could not be enabled
 */
TL::LedDriver::Error TL::LedDriver::startI2S(const TL::LedDriver::DMABuffer *startBuffer)
{
    TL::LedDriver::resetI2S();

    TL::LedDriver::i2sDevice->lc_conf.val = I2S_OUT_DATA_BURST_EN | I2S_OUTDSCR_BURST_EN | I2S_OUT_DATA_BURST_EN;
    TL::LedDriver::i2sDevice->out_link.addr = reinterpret_cast<uint32_t>(&(startBuffer->descriptor));
    TL::LedDriver::i2sDevice->out_link.start = 1;
    TL::LedDriver::i2sDevice->int_clr.val = TL::LedDriver::i2sDevice->int_raw.val;
    TL::LedDriver::i2sDevice->int_clr.val = TL::LedDriver::i2sDevice->int_raw.val;
    TL::LedDriver::i2sDevice->int_ena.val = 0;
    TL::LedDriver::i2sDevice->int_ena.out_eof = 1;
    TL::LedDriver::i2sDevice->int_ena.out_total_eof = 1;

    if (esp_intr_enable(TL::LedDriver::interruptHandle) != ESP_OK)
    {
        return TL::LedDriver::Error::ERROR_ENABLE_INTERRUPT;
    }

    TL::LedDriver::i2sDevice->conf.tx_start = 1;
    return TL::LedDriver::Error::OK;
}

/**
 * @brief Reset the I2S device.
 */
void IRAM_ATTR TL::LedDriver::resetI2S()
{
    const unsigned long lcConfigResetFlage = I2S_IN_RST_M | I2S_OUT_RST_M | I2S_AHBM_RST_M | I2S_AHBM_FIFO_RST_M;
    TL::LedDriver::i2sDevice->lc_conf.val |= lcConfigResetFlage;
    TL::LedDriver::i2sDevice->lc_conf.val &= ~lcConfigResetFlage;
    const uint32_t configResetFlags = I2S_RX_RESET_M | I2S_RX_FIFO_RESET_M | I2S_TX_RESET_M | I2S_TX_FIFO_RESET_M;
    TL::LedDriver::i2sDevice->conf.val |= configResetFlags;
    TL::LedDriver::i2sDevice->conf.val &= ~configResetFlags;
}

/**
 * @brief Stop the I2S device after all data was sent to the LEDs.
 */
void IRAM_ATTR TL::LedDriver::stopI2S()
{
    if (esp_intr_disable(TL::LedDriver::interruptHandle) != ESP_OK)
    {
        // Idk...
    }

    TL::LedDriver::i2sDevice->conf.tx_start = 0;
    // while (TL::LedDriver::i2sDevice->conf.tx_start == 1);
    resetI2S();
    ets_delay_us(30);
}

/**
 * @brief Reset the DMA of the I2S device.
 */
void TL::LedDriver::resetDMA()
{
    TL::LedDriver::i2sDevice->lc_conf.out_rst = 1;
    TL::LedDriver::i2sDevice->lc_conf.out_rst = 0;
}

/**
 * @brief Reset the FIFO of the I2S device.
 */
void TL::LedDriver::resetFIFO()
{
    TL::LedDriver::i2sDevice->conf.tx_fifo_reset = 1;
    TL::LedDriver::i2sDevice->conf.tx_fifo_reset = 0;
}

/**
 * @brief Allocate a DMA buffer with a given size.
 * @param size number of elements to allocate
 * @return allocated DMA buffer
 */
TL::LedDriver::DMABuffer *TL::LedDriver::allocateDMABuffer(const uint32_t size)
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
void TL::LedDriver::freeDMABuffer(TL::LedDriver::DMABuffer *dmaBuffer)
{
    heap_caps_free(dmaBuffer->buffer);
    heap_caps_free(dmaBuffer);
}

/**
 * @brief Initialize the DMA buffers.
 */
void TL::LedDriver::initDMABuffers()
{
    TL::LedDriver::dmaBuffer[0] = TL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3);
    TL::LedDriver::dmaBuffer[1] = TL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3);
    TL::LedDriver::dmaBuffer[2] = TL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3);
    TL::LedDriver::dmaBuffer[3] = TL::LedDriver::allocateDMABuffer(3 * 8 * 2 * 3 * 4);

    for (uint8_t i = 0; i < 2; i++)
    {
        for (uint8_t j = 0; j < 3 * 8 / 2; j++)
        {
            uint16_t *buffer = reinterpret_cast<uint16_t *>(TL::LedDriver::dmaBuffer[i]->buffer);
            buffer[j * 6 + 1] = 0xffff;
            buffer[j * 6 + 2] = 0xffff;
        }
    }
}

/**
 * @brief Interrupt handler is called once a buffer was sent. It will then preload the next buffer.
 * @param args arguments, currently not used
 */
void IRAM_ATTR TL::LedDriver::interruptHandler(void *args)
{
    if (GET_PERI_REG_BITS(I2S_INT_ST_REG(static_cast<uint8_t>(TL::LedDriver::i2sDeviceIdentifier)), I2S_OUT_EOF_INT_ST_V, I2S_OUT_EOF_INT_ST_S))
    {
        TL::LedDriver::ledIndex++;
        if (TL::LedDriver::ledIndex < TL::LedDriver::ledStripMaxLength)
        {
            loadDMABuffer(TL::LedDriver::ledBuffer, reinterpret_cast<uint16_t *>(TL::LedDriver::dmaBuffer[TL::LedDriver::dmaBufferIndex]->buffer), TL::LedDriver::ledStripLength, TL::LedDriver::ledStripCount, TL::LedDriver::ledIndex);

            if (TL::LedDriver::ledIndex == TL::LedDriver::ledStripMaxLength - 3)
            {
                TL::LedDriver::dmaBuffer[TL::LedDriver::dmaBufferIndex]->descriptor.qe.stqe_next = &(TL::LedDriver::dmaBuffer[3]->descriptor);
            }
            TL::LedDriver::dmaBufferIndex = (TL::LedDriver::dmaBufferIndex + 1) % 2;
        }
    }

    if (GET_PERI_REG_BITS(I2S_INT_ST_REG(static_cast<uint8_t>(TL::LedDriver::i2sDeviceIdentifier)), I2S_OUT_TOTAL_EOF_INT_ST_V, I2S_OUT_TOTAL_EOF_INT_ST_S))
    {
        TL::LedDriver::stopI2S();
        portBASE_TYPE hpTaskAwoken = pdFALSE;
        xSemaphoreGiveFromISR(TL::LedDriver::semaphore, &hpTaskAwoken);
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
void IRAM_ATTR TL::LedDriver::loadDMABuffer(uint8_t *ledBuffer, uint16_t *dmaBuffer, const uint16_t *ledStripLength, const uint16_t ledStripCount, const uint16_t ledIndex)
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
void IRAM_ATTR TL::LedDriver::transpose(uint8_t *pixelBuffer, uint16_t *dmaBuffer)
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
