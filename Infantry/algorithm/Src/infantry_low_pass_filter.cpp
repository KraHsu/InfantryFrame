/**
 * @file Infantry/algorithm/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/27/2023
 */
#include "infantry_low_pass_filter.h"

#define PI 3.14159265358979f

namespace infantry {

    LowPassFilter::LowPassFilter(float param) {
        _param.filt_para = param;
    }

    float LowPassFilter::calculate(float val) {
        uint32_t period = HAL_GetTick() - _param.last_tick;
        _param.last_tick = HAL_GetTick();
        if ((_param.filt_para > 0) && (_param.filt_para <= 1)) {
            filtered_val = _param.filt_para * val + (1 - _param.filt_para) * filtered_last_val;
            filtered_last_val = filtered_val;
            if (period > 0) {
                _param.cut_off_frq = _param.filt_para / (2 * PI * (float) period * 0.001f);
            }
            _param.calc_frq = 1000 / (float) period;
            return filtered_val;
        } else {
            return val;
        }
    }
}
