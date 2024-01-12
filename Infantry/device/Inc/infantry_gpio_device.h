/**
 * @file Infantry/device/Src
 * @author CharlesHsu
 * @date 12/19/2023
 */
#pragma once

#include "gpio.h"

namespace infantry {

    class GpioDevice {
    private:
        GPIO_TypeDef *_gpio;
        uint16_t _pin;
        GPIO_PinState _state;
    public:
        GpioDevice(GPIO_TypeDef *gpio, uint16_t pin, GPIO_PinState state = GPIO_PIN_RESET);

        GpioDevice *reset();

        GpioDevice *set();

        GpioDevice *toggle();

        GPIO_PinState read();

        ~GpioDevice() = default;
    };

} // infantry
