/*
 *                                                     __----~~~~~~~~~~~------___
 *                                    .  .   ~~//====......          __--~ ~~
 *                    -.            \_|//     |||\\  ~~~~~~::::... /~
 *                 ___-==_       _-~o~  \/    |||  \\            _/~~-
 *         __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
 *     _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
 *   .~       .~       |   \\ -_    /  /-   /   ||      \   /
 *  /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 *  |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
 *           '         ~-|      /|    |-~\~~       __--~~
 *                       |-~~-_/ |    |   ~\_   _-~            /\
 *                            /  \     \__   \/~                \__
 *                        _--~ _/ | .-~~____--~-/                  ~~==.
 *                       ((->/~   '.|||' -_|    ~~-/ ,              . _||
 *                                  -_     ~\      ~~---l__i__i__i--~~_/
 *                                  _-~-__   ~)  \--______________--~~
 *                                //.-~~~-~_--~- |-------~~~~~~~~
 *                                       //.-~~~--\
 *                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *                               神兽保佑            永无BUG
 *
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:50:40
 * Copyright (c) 2024 by KraHsu, All Rights Reserved.
 */

#pragma once

#include "infantry_kalman.h"

#define QEKF QuaternionExtendedKalmanFilter

namespace infantry
{
    class QEKF : public KalmanFilter
    {
    public:
        uint8_t _initialized{0};
        uint8_t _converge_flag{0};
        uint8_t _stable_flag{0};
        uint64_t _error_count{0};
        uint64_t _update_count{0};

        float _q[4]{0};
        float _gyro_bias[3]{0};

        float _gyro[3]{0};
        float _accel[3]{0};

        float _orientation_cosine[3]{0};

        float _acc_low_pass_filter_coef{0};
        float _gyro_norm{0};
        float _accl_norm{0};
        float _adaptive_gain_scale{0};

        float _roll{0};
        float _pitch{0};
        float _yaw{0};

        float _yaw_total_angle{0};

        float _q1{0};
        float _q2{0};
        float _r{0};

        float _dt{0};
        mat _chi_square{0};
        float _chi_square_data[1]{0};
        float _chi_square_test_threshold{0};
        float _lambda{0};

        int16_t _yaw_round_count{0};

        float _yaw_angle_last{0};
        float _qekf_k[18]{0};
        float _qekf_h[18]{0};
        float _qekf_P[36]{0};

        QEKF *_observe();

        QEKF *_fLinearizationPFading();

        QEKF *_setH();

        QEKF *_xHatUpdate() override;

    public:
        QEKF(float process_noise1, float process_noise2, float measure_noise, float lambda, float lpf);

        QEKF *init() override;

        QEKF *update(float gx, float gy, float gz, float ax, float ay, float az, float dt);
    };
}
