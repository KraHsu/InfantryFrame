/**
 * @file Infantry/Src
 * @author CharlesHsu
 * @date 11/12/2023
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

        virtual UsartDataType &init();

        virtual ~UsartDataType();
    };

    class UsartRxDataType : public UsartDataType {
    protected:
        HAL_StatusTypeDef _state{HAL_OK};
    public:
        friend class UsartDevice;

        using UsartDataType::UsartDataType;

        virtual UsartDataType &rxCallback();

        UsartRxDataType &init() override;
    };

    class UsartTxDataType : public UsartDataType {
    public:
        friend class UsartDevice;

        using UsartDataType::UsartDataType;

        virtual UsartDataType &txHook() = 0;
    };

    class UsartDevice {
    private:
        UsartRxDataType *_rx_data{nullptr};
        UsartTxDataType *_tx_data{nullptr};
    public:
        explicit UsartDevice(UsartRxDataType *rx_data, UsartTxDataType *tx_data);

        ~UsartDevice();

        UsartDevice &init();

        UsartDevice &registerRxCallback();

        UsartDevice &signOffRxCallback();

        UsartDevice &transmit(uint32_t timeout);

        HAL_StatusTypeDef getState();
    };
} // infantry
