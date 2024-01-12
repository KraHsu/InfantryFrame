/**
 * @file Infantry/device/Src
 * @author CharlesHsu
 * @date 12/19/2023
 */
#pragma once

#include "spi.h"

namespace infantry {

    class SpiDevice {
    private:
        SPI_HandleTypeDef *_hspi;
    public:
        /**
         * @brief New a SpiDevice
         * @param hspi Spi handler
         * @author CharlesHsu
         */
        explicit SpiDevice(SPI_HandleTypeDef *hspi);

        /**
         * @brief Init the spi device
         * @param config [SPI_InitTypeDef] The default value is {BaudRatePrescaler : SPI_BAUDRATEPRESCALER_8}
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *init(SPI_InitTypeDef config = {.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8});

        /**
         * @brief receive spi data (for DMA)
         * @param buffer Buffer pointer
         * @param len Buffer length
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *receiveDataDMA(uint8_t *buffer, uint16_t len);

        /**
         * @brief receive spi data
         * @param buffer Buffer pointer
         * @param len Buffer length
         * @param timeout [ms] Timeout duration. The default value is 10 ms
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *receiveData(uint8_t *buffer, uint16_t len, uint32_t timeout = 10);

        /**
         * @brief Send spi data (for DMA)
         * @param buffer Buffer pointer
         * @param len Buffer length
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *sendDataDMA(uint8_t *buffer, uint16_t len);

        /**
         * @brief Send spi data
         * @param buffer Buffer pointer
         * @param len Buffer length
         * @param timeout [ms] Timeout duration. The default value is 10 ms
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *sendData(uint8_t *buffer, uint16_t len, uint32_t timeout = 10);

        /**
         * @brief Swap one byte data or command to spi device
         * @param tx_data data to be sent
         * @param rx_data_ptr pointer to reception data buffer
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *swapOneByteData(uint8_t tx_data, uint8_t *rx_data_ptr);

        /**
         * @brief Swap multi data or command to spi address
         * @param command Data to transmit
         * @param rx_data_ptr Pointer to reception data buffer
         * @param length Length of reception data buffer
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *readMultiReg(uint8_t command, uint8_t *rx_data_ptr, uint8_t length);

        /**
         * @brief Swap data or command to spi address (for DMA)
         * @param tx_data_ptr Pointer to the transmission data buffer
         * @param rx_data_ptr Pointer to reception data buffer
         * @param len Length of data buffer
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *swapDataDMA(uint8_t *tx_data_ptr, uint8_t *rx_data_ptr, uint16_t len);

        /**
         * @brief Swap data or command to spi address
         * @param tx_data_ptr Pointer to the transmission data buffer
         * @param rx_data_ptr Pointer to reception data buffer
         * @param len Length of data buffer
         * @param timeout [ms] Timeout duration. The default value is 10 ms
         * @author CharlesHsu
         * @return this
         */
        SpiDevice *swapDataDMA(uint8_t *tx_data_ptr, uint8_t *rx_data_ptr, uint16_t len, uint32_t timeout = 10);

        ~SpiDevice() = default;
    };

} // infantry
