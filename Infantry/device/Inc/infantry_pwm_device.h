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

#include "tim.h"
#include "infantry_low_pass_filter.h"

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
        float _speed{};
        float _feedback{};
        LowPassFilter _speed_filter{0.05f};
    public:
        explicit PwmEncoder(TIM_HandleTypeDef *htim);

        PwmEncoder *init();

        PwmEncoder *start();

        PwmEncoder *update();
    };
} // infantry
