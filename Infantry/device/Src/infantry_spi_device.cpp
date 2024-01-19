/*
 *           佛曰:  
 *                   写字楼里写字间，写字间里程序员；  
 *                   程序人员写程序，又拿程序换酒钱。  
 *                   酒醒只在网上坐，酒醉还来网下眠；  
 *                   酒醉酒醒日复日，网上网下年复年。  
 *                   但愿老死电脑间，不愿鞠躬老板前；  
 *                   奔驰宝马贵者趣，公交自行程序员。  
 *                   别人笑我忒疯癫，我笑自己命太贱；  
 *                   不见满街漂亮妹，哪个归得程序员？
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:54:36
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
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
