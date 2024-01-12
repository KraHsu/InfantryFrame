/**
 * @file Infantry/device/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/12/2023
 */
#include "infantry_pwm_device.h"

namespace infantry {
    PwmDevice::PwmDevice(
            TIM_HandleTypeDef *htim, uint32_t channel, uint32_t clock, uint32_t period
    ) : _htim{htim}, _tim_channel{channel}, _duty{0.0}, _clock{clock}, _period{period}, _is_running{false} {
        _tim_config.OCMode = TIM_OCMODE_PWM1;
        _tim_config.OCPolarity = TIM_OCPOLARITY_HIGH;
        _tim_config.OCFastMode = TIM_OCFAST_DISABLE;
    }

    PwmDevice *PwmDevice::setFreq(uint32_t freq) {
        bool isRunning = _is_running;
        stop();
        _freq = freq;

        _htim->Init.Period = _period;
        _htim->Init.Prescaler = _clock / (_htim->Init.Period + 1) / _freq - 1;
        HAL_TIM_PWM_Init(_htim);

        setDuty(_duty);

        if (isRunning) {
            start();
        }
        return this;
    }

    PwmDevice *PwmDevice::setDuty(float duty) {
        bool last_state = _is_running;
        stop();
        _duty = duty;

        _tim_config.Pulse = uint32_t(_duty * float(_htim->Init.Period + 1));
        HAL_TIM_PWM_ConfigChannel(_htim, &_tim_config, _tim_channel);

        if (last_state) {
            start();
        }
        return this;
    }

    PwmDevice *PwmDevice::start() {
        if (!_is_running) {
            HAL_TIM_PWM_Start(_htim, _tim_channel);
            _is_running = true;
        }
        return this;
    }

    PwmDevice *PwmDevice::stop() {
        if (_is_running) {
            HAL_TIM_PWM_Stop(_htim, _tim_channel);
            _is_running = false;
        }
        return this;
    }

    void PwmEncoder::start() {
        HAL_TIM_Encoder_Start(_htim, TIM_CHANNEL_ALL);
    }

    PwmEncoder::PwmEncoder(TIM_HandleTypeDef *htim) : _htim{htim} {}
} // infantry
