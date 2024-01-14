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
    public:
        using StateEnum = enum {
            STOP = false,
            START = true
        };
    private:
        StateEnum _state{STOP};
        TIM_HandleTypeDef *_htim{nullptr};
        TIM_OC_InitTypeDef _config{};
        uint32_t _channel{};
        uint32_t _clock{};
        uint32_t _freq{};
        float _duty{};
    public:
        /**
         * @brief New a pwm device
         * @param htim Tim handler
         * @param channel Tim channel
         * @param clock Pwm clock
         * @author CharlesHsu
         */
        PwmDevice(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t clock);

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

        virtual /**
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
