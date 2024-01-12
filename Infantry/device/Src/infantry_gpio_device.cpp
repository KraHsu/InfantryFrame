/**
 * @file Infantry/device/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/19/2023
 */
#include "infantry_gpio_device.h"

namespace infantry {
    GpioDevice::GpioDevice(GPIO_TypeDef *gpio, uint16_t pin, GPIO_PinState state)
            : _gpio{gpio}, _pin{pin}, _state{state} {}

    GpioDevice *GpioDevice::reset() {
        HAL_GPIO_WritePin(_gpio, _pin, GPIO_PIN_RESET);
        read();
        return this;
    }

    GpioDevice *GpioDevice::set() {
        HAL_GPIO_WritePin(_gpio, _pin, GPIO_PIN_SET);
        read();
        return this;
    }

    GPIO_PinState GpioDevice::read() {
        _state = HAL_GPIO_ReadPin(_gpio, _pin);
        return _state;
    }

    GpioDevice *GpioDevice::toggle() {
        HAL_GPIO_TogglePin(_gpio, _pin);
        read();
        return this;
    }
} // infantry
