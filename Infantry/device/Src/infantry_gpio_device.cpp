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
 * @LastEditTime: 2024-01-17 01:53:38
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
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
