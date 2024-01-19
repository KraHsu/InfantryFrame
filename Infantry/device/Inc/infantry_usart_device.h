/*
 * _______________#########_______________________ 
 * ______________############_____________________ 
 * ______________#############____________________ 
 * _____________##__###########___________________ 
 * ____________###__######_#####__________________ 
 * ____________###_#######___####_________________ 
 * ___________###__##########_####________________ 
 * __________####__###########_####_______________ 
 * ________#####___###########__#####_____________ 
 * _______######___###_########___#####___________ 
 * _______#####___###___########___######_________ 
 * ______######___###__###########___######_______ 
 * _____######___####_##############__######______ 
 * ____#######__#####################_#######_____ 
 * ____#######__##############################____ 
 * ___#######__######_#################_#######___ 
 * ___#######__######_######_#########___######___ 
 * ___#######____##__######___######_____######___ 
 * ___#######________######____#####_____#####____ 
 * ____######________#####_____#####_____####_____ 
 * _____#####________####______#####_____###______ 
 * ______#####______;###________###______#________ 
 * ________##_______####________####______________ 
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:53:18
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include "usart.h"

#define DMA_SET_COUNTER(__HANDLE__, __COUNTER__) ((__HANDLE__)->Instance->CNDTR = (uint16_t)(__COUNTER__))

#ifdef __cplusplus
extern "C" {
void UartReceiveHandler(UART_HandleTypeDef *huart);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
};
#endif
/* Forward declaration */
namespace infantry {
    class UsartDevice;

    class UsartDataType;

    class UsartRxDataType;

    class UsartTxDataType;
}
/* Global variables and functions */
namespace infantry {
    extern UsartRxDataType *UsartDeviceRxDataMap[3];

    [[noreturn]] void UsartErrorHandler(const char *str);
}
/* Major classes */
namespace infantry {
    class UsartDataType {
    protected:
        UART_HandleTypeDef *_huart{nullptr};
        uint16_t _buffer_size{0};
        uint8_t *_buffer{nullptr};
    public:
        friend class UsartDevice;

        explicit UsartDataType(UART_HandleTypeDef *huart, uint16_t buffer_size);

        virtual UsartDataType *init();

        virtual ~UsartDataType();
    };

    class UsartRxDataType : public UsartDataType {
    protected:
        HAL_StatusTypeDef _state{HAL_OK};
    public:
        friend class UsartDevice;

        using UsartDataType::UsartDataType;

        virtual UsartDataType *rxCallback();

        UsartRxDataType *init() override;
    };

    class UsartTxDataType : public UsartDataType {
    public:
        friend class UsartDevice;

        using UsartDataType::UsartDataType;

        virtual UsartDataType *txHook() = 0;
    };

    class UsartDevice {
    protected:
        UsartRxDataType *_rx_data{nullptr};
        UsartTxDataType *_tx_data{nullptr};
    public:
        explicit UsartDevice(UsartRxDataType *rx_data, UsartTxDataType *tx_data);

        ~UsartDevice();

        virtual UsartDevice *init();

        virtual UsartDevice *registerRxCallback();

        virtual UsartDevice *signOffRxCallback();

        virtual UsartDevice *transmit(uint32_t timeout);

        HAL_StatusTypeDef getState();
    };
} // infantry
