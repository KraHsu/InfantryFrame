/**
 * @file Infantry/device/Src
 * @author CharlesHsu
 * @date 12/12/2023
 */
#pragma once

#include "tim.h"

#include <functional>

namespace infantry {
    class PwmDevice {
    private:
        bool _is_running;
        TIM_HandleTypeDef *_htim;
        uint32_t _tim_channel;
        uint32_t _clock;
        uint32_t _period;
        TIM_OC_InitTypeDef _tim_config{};
        float _duty;
        uint32_t _freq{};
    public:
        /**
         * @brief New a pwm device
         * @param htim Tim handler
         * @param channel Tim channel
         * @param clock Pwm clock
         * @param period Pwm period
         * @author CharlesHsu
         */
        PwmDevice(
                TIM_HandleTypeDef *htim, uint32_t channel, uint32_t clock = 170000000, uint32_t period = 99
        );

        /**
         * @brief Start the pwm device
         * @return this
         * @author CharlesHsu
         */
        PwmDevice *start();

        /**
         * @brief Stop the pwm device
         * @return this
         * @author CharlesHsu
         */
        PwmDevice *stop();

        /**
         * @brief Set duty
         * @return this
         * @author CharlesHsu
         */
        PwmDevice *setDuty(float duty);

        /**
         * @brief Set frequency
         * @return this
         * @author CharlesHsu
         */
        PwmDevice *setFreq(uint32_t freq);
    };

    class PwmEncoder {
    private:
        TIM_HandleTypeDef *_htim;
        uint8_t _direction{};
        uint16_t _counter{};
        uint32_t _last_update_time{};
    public:
        explicit PwmEncoder(TIM_HandleTypeDef *htim);

        void start();
    };
} // infantry
