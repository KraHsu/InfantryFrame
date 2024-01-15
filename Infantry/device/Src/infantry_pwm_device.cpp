/**
 * @file Infantry/device/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/12/2023
 */
#include "infantry_pwm_device.h"
#include "infantry_log.h"

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

    PwmEncoder *PwmEncoder::start() {
        if (HAL_TIM_Encoder_Start(_htim, TIM_CHANNEL_ALL) != HAL_OK) {
            logErrorWithTag("PwmEncoder", "HAL_TIM_Encoder_Start error");
            while (true) {

            }
        }
        return this;
    }

    PwmEncoder::PwmEncoder(TIM_HandleTypeDef *htim)
            : _htim{htim} {}

    PwmEncoder *PwmEncoder::init() {
        return this;
    }

    PwmEncoder *PwmEncoder::update() {
        uint8_t direction{};
        uint16_t counter{};
        uint16_t feedback{};

        direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(_htim);
        counter = __HAL_TIM_GET_COUNTER(_htim);

        if (direction == 0) {
            feedback = counter;
        } else {
            feedback = 65535 - counter;
        }
        if (feedback == 65535) {
            feedback = 0;
        }
        __HAL_TIM_SET_COUNTER(_htim, 0);

        _speed = float(feedback) / 2048 * 3.1415926f * 26;

        _feedback = _speed_filter.calculate(_speed);

        return this;
    }
} // infantry
