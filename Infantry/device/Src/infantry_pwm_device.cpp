/**
 * @file Infantry/device/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/12/2023
 */
#include "infantry_pwm_device.h"

namespace infantry {
    PwmDevice::PwmDevice(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t clock)
            : _htim{htim}, _channel{channel}, _duty{0.0}, _clock{clock}, _state{STOP} {
        _config.OCMode = TIM_OCMODE_PWM1;
        _config.OCPolarity = TIM_OCPOLARITY_HIGH;
        _config.OCFastMode = TIM_OCFAST_DISABLE;
    }

    PwmDevice *PwmDevice::start() {
        if (_state == STOP) {
            HAL_TIM_PWM_Start(_htim, _channel);
            _state = START;
        }
        return this;
    }

    PwmDevice *PwmDevice::stop() {
        if (_state == START) {
            HAL_TIM_PWM_Stop(_htim, _channel);
            _state = STOP;
        }
        return this;
    }

    PwmDevice *PwmDevice::setDuty(float duty) {
        StateEnum lastState = _state;
        stop();
        _duty = duty;

        _config.Pulse = uint32_t(_duty * float(_htim->Init.Period + 1));
        HAL_TIM_PWM_ConfigChannel(_htim, &_config, _channel);
        if (lastState == START) {
            start();
        }
        return this;
    }

    PwmDevice *PwmDevice::setFreq(uint32_t freq) {
        StateEnum lastState = _state;
        stop();
        _freq = freq;
        _htim->Init.Prescaler = _clock / (_htim->Init.Period + 1) / _freq - 1;
        HAL_TIM_PWM_Init(_htim);
        setDuty(_duty);
        if (lastState) {
            start();
        }
        return this;
    }

    void PwmEncoder::start() {
        HAL_TIM_Encoder_Start(_htim, TIM_CHANNEL_ALL);
    }

    PwmEncoder::PwmEncoder(TIM_HandleTypeDef *htim) : _htim{htim} {}
} // infantry
