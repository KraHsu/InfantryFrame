/*
 * 
 * 　　┏┓　　　┏┓+ +
 * 　┏┛┻━━━┛┻┓ + +
 * 　┃　　　　　　　┃ 　
 * 　┃　　　━　　　┃ ++ + + +
 *  ████━████ ┃+
 * 　┃　　　　　　　┃ +
 * 　┃　　　┻　　　┃
 * 　┃　　　　　　　┃ + +
 * 　┗━┓　　　┏━┛
 * 　　　┃　　　┃　　　　　　　　　　　
 * 　　　┃　　　┃ + + + +
 * 　　　┃　　　┃
 * 　　　┃　　　┃ +  神兽保佑
 * 　　　┃　　　┃    代码无bug　　
 * 　　　┃　　　┃　　+　　　　　　　　　
 * 　　　┃　 　　┗━━━┓ + +
 * 　　　┃ 　　　　　　　┣┓
 * 　　　┃ 　　　　　　　┏┛
 * 　　　┗┓┓┏━┳┓┏┛ + + + +
 * 　　　　┃┫┫　┃┫┫
 * 　　　　┗┻┛　┗┻┛+ + + +
 * 
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:47:00
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include "infantry_math.h"

namespace infantry
{
    class KalmanFilter
    {
    protected:
        float *_filtered_value{};
        float *_measured_vector{};
        float *_control_vector{};

        uint8_t _x_hat_size{};
        uint8_t _u_size{};
        uint8_t _z_size{};

        uint8_t _use_auto_adjustment{};
        uint8_t _measurement_valid_num{};

        uint8_t *_measurement_map{};       // how measurement relates to the state
        float *_measurement_degree{};      // elements of each measurement in H
        float *_mat_r_diagonal_elements{}; // variance for each measurement
        float *_state_min_variance{};      // suppress filter excessive convergence
        uint8_t *_temp{};
        uint8_t _non_measurement{};
        bool _skip_x_hat_minus_update{}, _skip_p_minus_update{}, _skip_set_k{}, _skip_x_hat_update{}, _skip_p_update{};

        mat _x_hat_mat{};        // x(k|k)
        mat _x_hat_minus_mat{};  // x(k|k-1)
        mat _u_mat{};            // control vector u
        mat _z_mat{};            // measurement vector z
        mat _p_mat{};            // covariance matrix P(k|k)
        mat _p_minus_mat{};      // covariance matrix P(k|k-1)
        mat _f_mat{}, _ft_mat{}; // state transition matrix F FT
        mat _b_mat{};            // control matrix B
        mat _h_mat{}, _ht_mat{}; // measurement matrix H
        mat _q_mat{};            // process noise covariance matrix Q
        mat _r_mat{};            // measurement noise covariance matrix R
        mat _k_mat{};            // kalman gain  K
        mat _s_mat{}, _temp_mat{}, _temp_mat1{}, _temp_vector{}, _temp_vector1{};

        int8_t _mat_status{};

        float *_x_hat_data{}, *_x_hat_minus_data{};
        float *_u_data{};
        float *_z_data{};
        float *_p_data{}, *_p_minus_data{};
        float *_f_data{}, *_ft_data{};
        float *_b_data{};
        float *_h_data{}, *_ht_data{};
        float *_q_data{};
        float *_r_data{};
        float *_k_data{};
        float *_s_data{}, *_temp_mat_data{}, *_temp_mat_data1{}, *_temp_vector_data{}, *_temp_vector_data1{};

        KalmanFilter *_measure();

        KalmanFilter *_minusUpdate();

        KalmanFilter *_pMinusUpdate();

        KalmanFilter *_setK();

        virtual KalmanFilter *_xHatUpdate();

        KalmanFilter *_pUpdate();

        KalmanFilter *_adjustmentHKR();

    public:
        KalmanFilter(uint8_t xhatSize, uint8_t uSize, uint8_t zSize);

        virtual KalmanFilter *init();

        KalmanFilter *update();
    };
}
