/**
 * @file Infantry/device/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/19/2023
 */
#include "infantry_spi_device.h"
#include "infantry_log.h"

[[noreturn]] void SPI_ErrorHandler(HAL_StatusTypeDef err) {
    logErrorWithTag("spi_d", "err(%d) occur when init the spi device!", err);
    while (true) {
        // TODO: Spi init error handler
    }
}

namespace infantry {
    SpiDevice::SpiDevice(SPI_HandleTypeDef *hspi) : _hspi{hspi} {}

    SpiDevice *SpiDevice::init(SPI_InitTypeDef config) {
        _hspi->Init = config;
        auto err = HAL_SPI_Init(_hspi);
        if (err != HAL_OK) {
            SPI_ErrorHandler(err);
        }

        return this;
    }

    SpiDevice *SpiDevice::receiveDataDMA(uint8_t *buffer, uint16_t len) {
        if ((_hspi == nullptr) || (buffer == nullptr)) {
            SPI_ErrorHandler(HAL_StatusTypeDef(0));
        }

        auto err = HAL_SPI_Receive_DMA(_hspi, buffer, len);
        if (err != HAL_OK) { SPI_ErrorHandler(err); }
        return this;
    }

    SpiDevice *SpiDevice::receiveData(uint8_t *buffer, uint16_t len, uint32_t timeout) {
        if ((_hspi == nullptr) || (buffer == nullptr)) {
            SPI_ErrorHandler(HAL_StatusTypeDef(0));
        }

        auto err = HAL_SPI_Receive(_hspi, buffer, len, timeout);
        if (err != HAL_OK) { SPI_ErrorHandler(err); }
        return this;
    }

    SpiDevice *SpiDevice::sendDataDMA(uint8_t *buffer, uint16_t len) {
        if ((_hspi == nullptr) || (buffer == nullptr)) {
            SPI_ErrorHandler(HAL_StatusTypeDef(0));
        }

        auto err = HAL_SPI_Transmit_DMA(_hspi, buffer, len);
        if (err != HAL_OK) { SPI_ErrorHandler(err); }
        return this;
    }

    SpiDevice *SpiDevice::sendData(uint8_t *buffer, uint16_t len, uint32_t timeout) {
        if ((_hspi == nullptr) || (buffer == nullptr)) {
            SPI_ErrorHandler(HAL_StatusTypeDef(0));
        }

        auto err = HAL_SPI_Transmit(_hspi, buffer, len, timeout);
        if (err != HAL_OK) { SPI_ErrorHandler(err); }
        return this;
    }

    SpiDevice *SpiDevice::swapOneByteData(uint8_t tx_data, uint8_t *rx_data_ptr) {
        auto ret = HAL_SPI_TransmitReceive(_hspi, &tx_data, rx_data_ptr, 1, 100);
        if (ret != HAL_OK) {
            SPI_ErrorHandler(ret);
        }
        return this;
    }

    SpiDevice *SpiDevice::readMultiReg(uint8_t command, uint8_t *rx_data_ptr, uint8_t length) {
        while (length--) {
            swapOneByteData(command, rx_data_ptr++);
        }
				return this;
    }

    SpiDevice *SpiDevice::swapDataDMA(uint8_t *tx_data_ptr, uint8_t *rx_data_ptr, uint16_t len) {
        if ((_hspi == nullptr) || (tx_data_ptr == nullptr) || (rx_data_ptr == nullptr)) {
            SPI_ErrorHandler(HAL_ERROR);
        }

        auto err = HAL_SPI_TransmitReceive_DMA(_hspi, tx_data_ptr, rx_data_ptr, len);
        if (err != HAL_OK) {
            SPI_ErrorHandler(err);
        }
        return this;
    }

    SpiDevice *SpiDevice::swapDataDMA(uint8_t *tx_data_ptr, uint8_t *rx_data_ptr, uint16_t len, uint32_t timeout) {
        if ((_hspi == nullptr) || (tx_data_ptr == nullptr) || (rx_data_ptr == nullptr)) {
            SPI_ErrorHandler(HAL_ERROR);
        }

        auto err = HAL_SPI_TransmitReceive(_hspi, tx_data_ptr, rx_data_ptr, len, timeout);
        if (err != HAL_OK) {
            SPI_ErrorHandler(err);
        }
        return this;
    }


} // infantry
