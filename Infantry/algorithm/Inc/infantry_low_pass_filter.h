/**
 * @file Infantry/algorithm/Src
 * @author CharlesHsu
 * @date 12/27/2023
 */
#pragma once

#include "main.h"

namespace infantry {
    class LowPassFilter {
    private:
        struct {
            float cut_off_frq{};
            float filt_para{};
            float last_tick{0};
            float calc_frq{};
        } _param;
        float filtered_val{};
        float filtered_last_val{};
    public:
        explicit LowPassFilter(float param);

        float calculate(float val);
    };
}
