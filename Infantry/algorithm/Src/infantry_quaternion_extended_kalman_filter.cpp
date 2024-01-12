/**
 * @file Infantry/algorithm/Src
 * @author CharlesHsu
 * @date 12/22/2023
 */

#include "infantry_quaternion_extended_kalman_filter.h"

constexpr float ConstQuaternionEkfF[36] = {
        1, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 1
};

constexpr float ConstQuaternionEkfP[36] = {
        1e5, 0.1, 0.1, 0.1, 0.1, 0.1,
        0.1, 1e5, 0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 1e5, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 1e5, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1, 1e2, 0.1,
        0.1, 0.1, 0.1, 0.1, 0.1, 1e2
};
namespace infantry {
    QEKF::QEKF(
            float process_noise1, float process_noise2, float measure_noise, float lambda, float lpf
    ) : KalmanFilter(6, 0, 3), _q1{process_noise1}, _q2{process_noise2}, _r{measure_noise},
        _lambda{lambda > 1 ? 1 : lambda}, _acc_low_pass_filter_coef{lpf} {
        for (int i = 0; i < 36; i++) {
            this->_qekf_P[i] = ConstQuaternionEkfP[i];
        }
    }

    QEKF *QEKF::init() {
        _initialized = 1;

        _chi_square_test_threshold = 1e-8;
        _converge_flag = 0;
        _error_count = 0;
        _update_count = 0;

        KalmanFilter::init();

        Matrix_Init(&_chi_square, 1, 1, (float *) _chi_square_data);

        _x_hat_data[0] = 1;
        _x_hat_data[1] = 0;
        _x_hat_data[2] = 0;
        _x_hat_data[3] = 0;

        _skip_set_k = true;
        _skip_x_hat_update = true;

        memcpy(_f_data, ConstQuaternionEkfF, sizeof(ConstQuaternionEkfF));
        memcpy(_p_data, _qekf_P, sizeof(_qekf_P));
        return this;
    }

    QEKF *QEKF::update(
            float gx, float gy, float gz, float ax, float ay, float az, float dt
    ) {
        // 0.5(Ohm-Ohm^bias)*deltaT
        static float halfGxDt, halfGyDt, halfGzDt;
        static float accelInvNorm;
        if (!_initialized) {
            QEKF::init();
        }

        /*   F, number with * represent vals to be set
         0      1*     2*     3*     4     5
         6*     7      8*     9*    10    11
        12*    13*    14     15*    16    17
        18*    19*    20*    21     22    23
        24     25     26     27     28    29
        30     31     32     33     34    35
        */
        dt = dt;

        _gyro[0] = gx - _gyro_bias[0];
        _gyro[1] = gy - _gyro_bias[1];
        _gyro[2] = gz - _gyro_bias[2];

        // set F
        halfGxDt = 0.5f * _gyro[0] * dt;
        halfGyDt = 0.5f * _gyro[1] * dt;
        halfGzDt = 0.5f * _gyro[2] * dt;

        memcpy(_f_data, ConstQuaternionEkfF, sizeof(ConstQuaternionEkfF));

        _f_data[1] = -halfGxDt;
        _f_data[2] = -halfGyDt;
        _f_data[3] = -halfGzDt;

        _f_data[6] = halfGxDt;
        _f_data[8] = halfGzDt;
        _f_data[9] = -halfGyDt;

        _f_data[12] = halfGyDt;
        _f_data[13] = -halfGzDt;
        _f_data[15] = halfGxDt;

        _f_data[18] = halfGzDt;
        _f_data[19] = halfGyDt;
        _f_data[20] = -halfGxDt;

        _observe();

        if (_update_count == 0) {
            _accel[0] = ax;
            _accel[1] = ay;
            _accel[2] = az;
        }
        _accel[0] = _accel[0] * _acc_low_pass_filter_coef / (dt + _acc_low_pass_filter_coef) +
                    ax * dt / (dt + _acc_low_pass_filter_coef);
        _accel[1] = _accel[1] * _acc_low_pass_filter_coef / (dt + _acc_low_pass_filter_coef) +
                    ay * dt / (dt + _acc_low_pass_filter_coef);
        _accel[2] = _accel[2] * _acc_low_pass_filter_coef / (dt + _acc_low_pass_filter_coef) +
                    az * dt / (dt + _acc_low_pass_filter_coef);

        accelInvNorm = Math_InvSqrt(
                _accel[0] * _accel[0] + _accel[1] * _accel[1] +
                _accel[2] * _accel[2]
        );
        for (uint8_t i = 0; i < 3; i++) {
            _measured_vector[i] = _accel[i] * accelInvNorm;
        }

        // get body state
        _gyro_norm = 1.0f / Math_InvSqrt(
                _gyro[0] * _gyro[0] +
                _gyro[1] * _gyro[1] +
                _gyro[2] * _gyro[2]
        );
        _accl_norm = 1.0f / accelInvNorm;

        if (_gyro_norm < 0.3f && _accl_norm > 9.8f - 0.5f && _accl_norm < 9.8f + 0.5f) {
            _stable_flag = 1;
        } else {
            _stable_flag = 0;
        }

        // set Q R
        _q_data[0] = _q1 * dt;
        _q_data[7] = _q1 * dt;
        _q_data[14] = _q1 * dt;
        _q_data[21] = _q1 * dt;
        _q_data[28] = _q2 * dt;
        _q_data[35] = _q2 * dt;
        _r_data[0] = _r;
        _r_data[4] = _r;
        _r_data[8] = _r;

        {
            _measure();
            _observe();

            // 1. xhat'(k)= A·xhat(k-1) + B·u
            _minusUpdate();
            _fLinearizationPFading();

            // 2. P'(k) = A·P(k-1)·AT + Q
            _pMinusUpdate();
            _setH();

            if (_measurement_valid_num != 0 || _use_auto_adjustment == 0) {
                // 3. K(k) = P'(k)·HT / (H·P'(k)·HT + R)
                _setK();

                QEKF::_xHatUpdate();

                // 4. xhat(k) = xhat'(k) + K(k)·(z(k) - H·xhat'(k))
                KalmanFilter::_xHatUpdate();

                // 5. P(k) = (1-K(k)·H)·P'(k) ==> P(k) = P'(k)-K(k)·H·P'(k)
                _pUpdate();
            } else {
                // xhat(k) = xhat'(k)
                // P(k) = P'(k)
                memcpy(_x_hat_data, _x_hat_minus_data, sizeof(float) * _x_hat_size);
                memcpy(_p_data, _p_minus_data, sizeof(float) * _x_hat_size * _x_hat_size);
            }

            // suppress filter excessive convergence
            for (uint8_t i = 0; i < _x_hat_size; i++) {
                if (_p_data[i * _x_hat_size + i] < _state_min_variance[i]) {
                    _p_data[i * _x_hat_size + i] = _state_min_variance[i];
                }
            }

            memcpy(_filtered_value, _x_hat_data, sizeof(float) * _x_hat_size);
        }

        _q[0] = _filtered_value[0];
        _q[1] = _filtered_value[1];
        _q[2] = _filtered_value[2];
        _q[3] = _filtered_value[3];
        _gyro_bias[0] = _filtered_value[4];
        _gyro_bias[1] = _filtered_value[5];
        _gyro_bias[2] = 0;

        _yaw = atan2f(
                2.0f * (_q[0] * _q[3] + _q[1] * _q[2]),
                2.0f * (_q[0] * _q[0] + _q[1] * _q[1]) - 1.0f
        ) * 57.295779513f;
        _pitch = atan2f(
                2.0f * (_q[0] * _q[1] + _q[2] * _q[3]),
                2.0f * (_q[0] * _q[0] + _q[3] * _q[3]) - 1.0f
        ) * 57.295779513f;
        _roll = asinf(-2.0f * (_q[1] * _q[3] - _q[0] * _q[2])) * 57.295779513f;

        if (_yaw - _yaw_angle_last > 180.0f) {
            _yaw_round_count--;
        } else if (_yaw - _yaw_angle_last < -180.0f) {
            _yaw_round_count++;
        }
        _yaw_total_angle = 360.0f * float(_yaw_round_count) + _yaw;
        _yaw_angle_last = _yaw;
        _update_count++;
        return this;
    }

    QEKF *QuaternionExtendedKalmanFilter::_observe() {
        memcpy(_qekf_P, _p_data, sizeof(_qekf_P));
        memcpy(_qekf_k, _k_data, sizeof(_qekf_k));
        memcpy(_qekf_h, _h_data, sizeof(_qekf_h));
        return this;
    }

    QEKF *QuaternionExtendedKalmanFilter::_fLinearizationPFading() {
        static float q0, q1, q2, q3;
        static float qInvNorm;

        q0 = _x_hat_minus_data[0];
        q1 = _x_hat_minus_data[1];
        q2 = _x_hat_minus_data[2];
        q3 = _x_hat_minus_data[3];

        // quaternion normalize
        qInvNorm = Math_InvSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
        for (uint8_t i = 0; i < 4; i++) {
            _x_hat_minus_data[i] *= qInvNorm;
        }
        /*  F, number with * represent vals to be set
         0     1     2     3     4*     5*
         6     7     8     9    10*    11*
        12    13    14    15    16*    17*
        18    19    20    21    22*    23*
        24    25    26    27    28     29
        30    31    32    33    34     35
        */
        // set F
        _f_data[4] = q1 * _dt / 2;
        _f_data[5] = q2 * _dt / 2;

        _f_data[10] = -q0 * _dt / 2;
        _f_data[11] = q3 * _dt / 2;

        _f_data[16] = -q3 * _dt / 2;
        _f_data[17] = -q0 * _dt / 2;

        _f_data[22] = q2 * _dt / 2;
        _f_data[23] = -q1 * _dt / 2;

        // fading filter
        _p_data[28] /= _lambda;
        _p_data[35] /= _lambda;

        if (_p_data[28] > 10000) {
            _p_data[28] = 10000;
        }
        if (_p_data[35] > 10000) {
            _p_data[35] = 10000;
        }
        return this;
    }

    QEKF *QuaternionExtendedKalmanFilter::_setH() {
        static float doubleq0, doubleq1, doubleq2, doubleq3;
        uint8_t sizeof_float = sizeof(float);
        /* H
         0     1     2     3     4     5
         6     7     8     9    10    11
        12    13    14    15    16    17
        last two cols are zero
        */
        // set H
        doubleq0 = 2 * _x_hat_minus_data[0];
        doubleq1 = 2 * _x_hat_minus_data[1];
        doubleq2 = 2 * _x_hat_minus_data[2];
        doubleq3 = 2 * _x_hat_minus_data[3];

        memset(_h_data, 0, sizeof_float * _z_size * _x_hat_size);

        _h_data[0] = -doubleq2;
        _h_data[1] = doubleq3;
        _h_data[2] = -doubleq0;
        _h_data[3] = doubleq1;

        _h_data[6] = doubleq1;
        _h_data[7] = doubleq0;
        _h_data[8] = doubleq3;
        _h_data[9] = doubleq2;

        _h_data[12] = doubleq0;
        _h_data[13] = -doubleq1;
        _h_data[14] = -doubleq2;
        _h_data[15] = doubleq3;
        return this;
    }

    QEKF *QuaternionExtendedKalmanFilter::_xHatUpdate() {
        static float q0, q1, q2, q3;
        uint8_t sizeof_float = sizeof(float);

        _mat_status = Matrix_Transpose(&_h_mat, &_ht_mat); // z|x => x|z
        _temp_mat.numRows = _h_mat.numRows;
        _temp_mat.numCols = _p_minus_mat.numCols;
        _mat_status = Matrix_Multiply(&_h_mat, &_p_minus_mat, &_temp_mat); // temp_matrix = H·P'(k)
        _temp_mat1.numRows = _temp_mat.numRows;
        _temp_mat1.numCols = _ht_mat.numCols;
        _mat_status = Matrix_Multiply(&_temp_mat, &_ht_mat, &_temp_mat1); // temp_matrix1 = H·P'(k)·HT
        _s_mat.numRows = _r_mat.numRows;
        _s_mat.numCols = _r_mat.numCols;
        _mat_status = Matrix_Add(&_temp_mat1, &_r_mat, &_s_mat); // S = H P'(k) HT + R
        _mat_status = Matrix_Inverse(&_s_mat, &_temp_mat1);     // temp_matrix1 = inv(H·P'(k)·HT + R)

        q0 = _x_hat_minus_data[0];
        q1 = _x_hat_minus_data[1];
        q2 = _x_hat_minus_data[2];
        q3 = _x_hat_minus_data[3];

        _temp_vector.numRows = _h_mat.numRows;
        _temp_vector.numCols = 1;

        _temp_vector_data[0] = 2 * (q1 * q3 - q0 * q2);
        _temp_vector_data[1] = 2 * (q0 * q1 + q2 * q3);
        _temp_vector_data[2] = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3; // temp_vector = h(xhat'(k))

        for (uint8_t i = 0; i < 3; i++) {
            _orientation_cosine[i] = acosf(fabsf(_temp_vector_data[i]));
        }

        _temp_vector1.numRows = _z_mat.numRows;
        _temp_vector1.numCols = 1;
        _mat_status = Matrix_Subtract(&_z_mat, &_temp_vector, &_temp_vector1); // temp_vector1 = z(k) - h(xhat'(k))

        // chi-square test
        _temp_mat.numRows = _temp_vector1.numRows;
        _temp_mat.numCols = 1;
        _mat_status = Matrix_Multiply(
                &_temp_mat1, &_temp_vector1, &_temp_mat
        ); // temp_matrix = inv(H·P'(k)·HT + R)·(z(k) - h(xhat'(k)))
        _temp_vector.numRows = 1;
        _temp_vector.numCols = _temp_vector1.numRows;
        _mat_status = Matrix_Transpose(&_temp_vector1, &_temp_vector); // temp_vector = z(k) - h(xhat'(k))'
        _mat_status = Matrix_Multiply(&_temp_vector, &_temp_mat, &_chi_square);
        // rk is small,filter converged/converging
        if (_chi_square_data[0] < 0.5f * _chi_square_test_threshold) {
            _converge_flag = 1;
        }
        // rk is bigger than thre but once converged
        if (_chi_square_data[0] > _chi_square_test_threshold && _converge_flag) {
            if (_stable_flag) {
                _error_count++;
            } else {
                _error_count = 0;
            }

            if (_error_count > 50) {
                _converge_flag = 0;
                _skip_p_update = false; // step-5 is cov mat P updating
            } else {
                //  xhat(k) = xhat'(k)
                //  P(k) = P'(k)
                memcpy(_x_hat_data, _x_hat_minus_data, sizeof_float * _x_hat_size);
                memcpy(_p_data, _p_minus_data, sizeof_float * _x_hat_size * _x_hat_size);
                _skip_p_update = true; // part5 is P updating
                return this;
            }
        } else { // if divergent or rk is not that big/acceptable,use adaptive gain
            // scale adaptive
            if (_chi_square_data[0] > 0.1f * _chi_square_test_threshold && _converge_flag) {
                _adaptive_gain_scale =
                        (_chi_square_test_threshold - _chi_square_data[0]) / (0.9f * _chi_square_test_threshold);
            } else {
                _adaptive_gain_scale = 1;
            }
            _error_count = 0;
            _skip_p_update = false;
        }

        // cal kf-gain K
        _temp_mat.numRows = _p_minus_mat.numRows;
        _temp_mat.numCols = _ht_mat.numCols;
        _mat_status = Matrix_Multiply(&_p_minus_mat, &_ht_mat, &_temp_mat); // temp_matrix = P'(k)·HT
        _mat_status = Matrix_Multiply(&_temp_mat, &_temp_mat1, &_k_mat);

        // implement adaptive
        for (uint32_t i = 0; i < _k_mat.numRows * _k_mat.numCols; i++) {
            _k_data[i] *= _adaptive_gain_scale;
        }
        for (uint32_t i = 4; i < 6; i++) {
            for (uint8_t j = 0; j < 3; j++) {
                _k_data[i * 3 + j] *= _orientation_cosine[i - 4] / 1.5707963f; // 1 rad
            }
        }

        _temp_vector.numRows = _k_mat.numRows;
        _temp_vector.numCols = 1;
        _mat_status = Matrix_Multiply(
                &_k_mat, &_temp_vector1, &_temp_vector
        ); // temp_vector = K(k)·(z(k) - H·xhat'(k))

        if (_converge_flag) {
            for (uint8_t i = 4; i < 6; i++) {
                if (_temp_vector.pData[i] > 1e-2f * _dt) {
                    _temp_vector.pData[i] = 1e-2f * _dt;
                }
                if (_temp_vector.pData[i] < -1e-2f * _dt) {
                    _temp_vector.pData[i] = -1e-2f * _dt;
                }
            }
        }

        _temp_vector.pData[3] = 0;
        _mat_status = Matrix_Add(&_x_hat_minus_mat, &_temp_vector, &_x_hat_mat);
        return this;
    }
}
