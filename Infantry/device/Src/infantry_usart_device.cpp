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
 * @LastEditTime: 2024-01-17 01:53:57
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#include "device/Inc/infantry_usart_device.h"
#include "infantry_log.h"

namespace infantry {
    UsartRxDataType *UsartDeviceRxDataMap[3]{nullptr};

    [[noreturn]] void UsartErrorHandler(const char *str) {
        logErrorWithTag("usart", str);
        while (true) {
            //TODO: Usart Devices Error Handler
        }
    }

    uint8_t GetUsartIndex(UART_HandleTypeDef *huart) {
        if (huart == &huart1) {
            return 0;
        } else if (huart == &huart2) {
            return 1;
        } else if (huart == &huart3) {
            return 2;
        }
        UsartErrorHandler("error handler");
    }

    void UsartGlobalCallBack(UART_HandleTypeDef *huart) {
        auto index = GetUsartIndex(huart);
        if (UsartDeviceRxDataMap[index] != nullptr) {
            UsartDeviceRxDataMap[index]->rxCallback();
        }
    }

    UsartDataType *UsartDataType::init() {
        _buffer = new uint8_t[_buffer_size];
        return this;
    }

    UsartDataType::~UsartDataType() {
        delete[] _buffer;
    }

    UsartDataType::UsartDataType(UART_HandleTypeDef *huart, uint16_t buffer_size)
            : _buffer_size(buffer_size), _huart{huart} {}

    UsartDevice::UsartDevice(UsartRxDataType *rx_data, UsartTxDataType *tx_data)
            : _rx_data{rx_data}, _tx_data{tx_data} {}

    UsartDevice::~UsartDevice() {
        delete _rx_data;
        delete _tx_data;
    }

    UsartDevice *UsartDevice::init() {
        if (_rx_data != nullptr) {
            _rx_data->init();
        }
        if (_tx_data != nullptr) {
            _tx_data->init();
        }
        if (getState() != HAL_OK) {
            UsartErrorHandler("init failed");
        }
        return this;
    }

    UsartDevice *UsartDevice::registerRxCallback() {
        if (_rx_data == nullptr) {
            UsartErrorHandler("Cannot register a receive callback function when the receive data type is null");
        }
        UsartDeviceRxDataMap[GetUsartIndex(_rx_data->_huart)] = _rx_data;
        return this;
    }

    UsartDevice *UsartDevice::signOffRxCallback() {
        UsartDeviceRxDataMap[GetUsartIndex(_rx_data->_huart)] = nullptr;
        return this;
    }

    UsartDevice *UsartDevice::transmit(uint32_t timeout) {
        if (_tx_data == nullptr) {
            UsartErrorHandler("Data cannot be sent if the send data type is null");
        }
        _tx_data->txHook();
        if (HAL_UART_Transmit(_tx_data->_huart, _tx_data->_buffer, _tx_data->_buffer_size, timeout) != HAL_OK) {
            logErrorWithTag("usart", "Data sending failure");
        }
        return this;
    }

    HAL_StatusTypeDef UsartDevice::getState() {
        return _rx_data->_state;
    }

    UsartDataType *UsartRxDataType::rxCallback() {
        __HAL_DMA_DISABLE(_huart->hdmarx);
        int rxDataLen = _buffer_size - uint16_t(__HAL_DMA_GET_COUNTER(_huart->hdmarx));
        //rxCallback(rxDataLen);
        DMA_SET_COUNTER(_huart->hdmarx, _buffer_size);
        __HAL_DMA_ENABLE(_huart->hdmarx);
        return this;
    }

    UsartRxDataType *UsartRxDataType::init() {
        UsartDataType::init();
        __HAL_UART_CLEAR_IDLEFLAG(_huart);
        __HAL_UART_ENABLE_IT(_huart, UART_IT_IDLE);

        if (_huart->RxState == HAL_UART_STATE_READY) {
            if ((_buffer == nullptr) || (_buffer_size == 0U)) {
                UsartErrorHandler("DMA cannot be enabled when the receive buffer is empty");
            }

            if ((_huart)->Lock == HAL_LOCKED) {
                _state = HAL_BUSY;
                return this;
            } else {
                _huart->Lock = HAL_LOCKED;
            }

            /* Set Reception type to Standard reception */
            _huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

            if (_huart->Instance != LPUART1) {
                /* Check that USART RTOEN bit is set */
                if (READ_BIT(_huart->Instance->CR2, USART_CR2_RTOEN) != 0U) {
                    /* Enable the UART Receiver Timeout Interrupt */
                    ATOMIC_SET_BIT(_huart->Instance->CR1, USART_CR1_RTOIE);
                }
            }

            _state = UART_Start_Receive_DMA(_huart, _buffer, _buffer_size);
            if (_state != HAL_OK) {
                UsartErrorHandler("Error occur");
            }
        } else {
            _state = HAL_BUSY;
            UsartErrorHandler("DMA not ready");
        }
        return this;
    }
} // infantry

void UartReceiveHandler(UART_HandleTypeDef *huart) {
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        infantry::UsartGlobalCallBack(huart);
    }
}

#ifdef __cplusplus
extern "C" {
void USART1_IRQHandler() {
    UartReceiveHandler(&huart1);
    HAL_UART_IRQHandler(&huart1);
}

void USART2_IRQHandler() {
    UartReceiveHandler(&huart2);
    HAL_UART_IRQHandler(&huart2);
}

void USART3_IRQHandler() {
    UartReceiveHandler(&huart3);
    HAL_UART_IRQHandler(&huart3);
}
};
#endif
