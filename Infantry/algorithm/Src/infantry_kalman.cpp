/*
 * @Project: Infantry Code
 *
 * @Author: GDDG08
 * @Date: 2021-12-31 17:37:14
 * @LastEditors: Hatrix
 * @LastEditTime: 2023-12-14 14:10:23
 */

#include "infantry_kalman.h"

namespace infantry {

    KalmanFilter::KalmanFilter(uint8_t xhatSize, uint8_t uSize, uint8_t zSize)
            : _x_hat_size{xhatSize}, _u_size{uSize}, _z_size{zSize} {
        _measurement_valid_num = 0;
        _non_measurement = 0;

        _skip_x_hat_minus_update = false;
        _skip_p_minus_update = false;
        _skip_set_k = false;
        _skip_x_hat_update = false;
        _skip_p_update = false;
    }

    KalmanFilter *KalmanFilter::init() {
        // measurement flags
        _measurement_map = new uint8_t[_z_size];
        memset(_measurement_map, 0, sizeof(uint8_t) * _z_size);
        _measurement_degree = new float[_z_size];
        memset(_measurement_degree, 0, sizeof(float) * _z_size);
        _mat_r_diagonal_elements = new float[_z_size]();
        _state_min_variance = new float[_x_hat_size]();
        _temp = new uint8_t[_z_size]();

        // filter data
        _filtered_value = new float[_x_hat_size]();
        _measured_vector = new float[_z_size]();
        _control_vector = new float[_u_size]();

        // xhat x(k|k)
        _x_hat_data = new float[_x_hat_size]();
        Matrix_Init(&_x_hat_mat, _x_hat_size, 1, _x_hat_data);

        // xhatminus x(k|k-1)
        _x_hat_minus_data = new float[_x_hat_size]();
        Matrix_Init(&_x_hat_minus_mat, _x_hat_size, 1, _x_hat_minus_data);

        if (_u_size != 0) {
            // control vector u
            _u_data = new float[_u_size]();
            Matrix_Init(&_u_mat, _u_size, 1, _u_data);
        }

        // measurement vector z
        _z_data = new float[_z_size]();
        Matrix_Init(&_z_mat, _z_size, 1, _z_data);

        // covariance matrix P(k|k)
        _p_data = new float[_x_hat_size * _x_hat_size]();
        Matrix_Init(&_p_mat, _x_hat_size, _x_hat_size, _p_data);

        // create covariance matrix P(k|k-1)
        _p_minus_data = new float[_x_hat_size * _x_hat_size]();
        Matrix_Init(&_p_minus_mat, _x_hat_size, _x_hat_size, _p_minus_data);

        // state transition matrix F FT
        _f_data = new float[_x_hat_size * _x_hat_size]();
        _ft_data = new float[_x_hat_size * _x_hat_size]();
        Matrix_Init(&_f_mat, _x_hat_size, _x_hat_size, _f_data);
        Matrix_Init(&_ft_mat, _x_hat_size, _x_hat_size, _ft_data);

        if (_u_size != 0) {
            // control matrix B
            _b_data = new float[_x_hat_size * _u_size]();
            Matrix_Init(&_b_mat, _x_hat_size, _u_size, _b_data);
        }

        // measurement matrix H
        _h_data = new float[_z_size * _x_hat_size]();
        _ht_data = new float[_x_hat_size * _z_size]();
        Matrix_Init(&_h_mat, _z_size, _x_hat_size, _h_data);
        Matrix_Init(&_ht_mat, _x_hat_size, _z_size, _ht_data);

        // process noise covariance matrix Q
        _q_data = new float[_x_hat_size * _x_hat_size]();
        Matrix_Init(&_q_mat, _x_hat_size, _x_hat_size, _q_data);

        // measurement noise covariance matrix R
        _r_data = new float[_z_size * _z_size]();
        Matrix_Init(&_r_mat, _z_size, _z_size, _r_data);

        // kalman gain K
        _k_data = new float[_x_hat_size * _z_size]();
        Matrix_Init(&_k_mat, _x_hat_size, _z_size, _k_data);

        _s_data = new float[_x_hat_size * _x_hat_size]();
        _temp_mat_data = new float[_x_hat_size * _x_hat_size]();
        _temp_mat_data1 = new float[_x_hat_size * _x_hat_size]();
        _temp_vector_data = new float[_x_hat_size]();
        _temp_vector_data1 = new float[_x_hat_size]();
        Matrix_Init(&_s_mat, _x_hat_size, _x_hat_size, _s_data);
        Matrix_Init(&_temp_mat, _x_hat_size, _x_hat_size, _temp_mat_data);
        Matrix_Init(&_temp_mat1, _x_hat_size, _x_hat_size, _temp_mat_data1);
        Matrix_Init(&_temp_vector, _x_hat_size, 1, _temp_vector_data);
        Matrix_Init(&_temp_vector1, _x_hat_size, 1, _temp_vector_data1);
        return this;
    }

    KalmanFilter *KalmanFilter::_measure() {
        if (_use_auto_adjustment != 0) {
            _adjustmentHKR();
        } else {
            memcpy(_z_data, _measured_vector, sizeof(float) * _z_size);
            memset(_measured_vector, 0, sizeof(float) * _z_size);
        }

        memcpy(_u_data, _control_vector, sizeof(float) * _u_size);
        return this;
    }

    KalmanFilter *KalmanFilter::_adjustmentHKR() {
        _measurement_valid_num = 0;

        memcpy(_z_data, _measured_vector, sizeof(float) * _z_size);
        memset(_measured_vector, 0, sizeof(float) * _z_size);

        // recognize measurement validity and adjust matrices H R K
        memset(_r_data, 0, sizeof(float) * _z_size * _z_size);
        memset(_h_data, 0, sizeof(float) * _x_hat_size * _z_size);
        for (uint8_t i = 0; i < _z_size; i++) {
            if (_z_data[i] != 0) {
                // rebuild vector z
                _z_data[_measurement_valid_num] = _z_data[i];
                _temp[_measurement_valid_num] = i;
                // rebuild matrix H
                _h_data[_x_hat_size * _measurement_valid_num + _measurement_map[i] - 1] = _measurement_degree[i];
                _measurement_valid_num++;
            }
        }
        for (uint8_t i = 0; i < _measurement_valid_num; i++) {

            // rebuild matrix R
            _r_data[i * _measurement_valid_num + i] = _mat_r_diagonal_elements[_temp[i]];
        }

        // adjust the dimensions of system matrices
        _h_mat.numRows = _measurement_valid_num;
        _h_mat.numCols = _x_hat_size;
        _ht_mat.numRows = _x_hat_size;
        _ht_mat.numCols = _measurement_valid_num;
        _r_mat.numRows = _measurement_valid_num;
        _r_mat.numCols = _measurement_valid_num;
        _k_mat.numRows = _x_hat_size;
        _k_mat.numCols = _measurement_valid_num;
        _z_mat.numRows = _measurement_valid_num;
        return this;
    }

    KalmanFilter *KalmanFilter::_minusUpdate() {
        if (!_skip_x_hat_minus_update) {
            if (_u_size > 0) {
                _temp_vector.numRows = _x_hat_size;
                _temp_vector.numCols = 1;
                _mat_status = Matrix_Multiply(&_f_mat, &_x_hat_mat, &_temp_vector);
                _temp_vector1.numRows = _x_hat_size;
                _temp_vector1.numCols = 1;
                _mat_status = Matrix_Multiply(&_b_mat, &_u_mat, &_temp_vector1);
                _mat_status = Matrix_Add(&_temp_vector, &_temp_vector1, &_x_hat_minus_mat);
            } else {
                _mat_status = Matrix_Multiply(&_f_mat, &_x_hat_mat, &_x_hat_minus_mat);
            }
        }
        return this;
    }

    KalmanFilter *KalmanFilter::_pMinusUpdate() {
        if (!_skip_p_minus_update) {
            _mat_status = Matrix_Transpose(&_f_mat, &_ft_mat);
            _mat_status = Matrix_Multiply(&_f_mat, &_p_mat, &_p_minus_mat);
            _temp_mat.numRows = _p_minus_mat.numRows;
            _temp_mat.numCols = _ft_mat.numCols;
            _mat_status = Matrix_Multiply(&_p_minus_mat, &_ft_mat, &_temp_mat); // temp_matrix = F P(k-1) FT
            _mat_status = Matrix_Add(&_temp_mat, &_q_mat, &_p_minus_mat);
        }
        return this;
    }

    KalmanFilter *KalmanFilter::_setK() {
        if (!_skip_set_k) {
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
            _temp_mat.numRows = _p_minus_mat.numRows;
            _temp_mat.numCols = _ht_mat.numCols;
            _mat_status = Matrix_Multiply(&_p_minus_mat, &_ht_mat, &_temp_mat); // temp_matrix = P'(k)·HT
            _mat_status = Matrix_Multiply(&_temp_mat, &_temp_mat1, &_k_mat);
        }
        return this;
    }

    KalmanFilter *KalmanFilter::_xHatUpdate() {
        if (!_skip_x_hat_update) {
            _temp_vector.numRows = _h_mat.numRows;
            _temp_vector.numCols = 1;
            _mat_status = Matrix_Multiply(&_h_mat, &_x_hat_minus_mat, &_temp_vector); // temp_vector = H xhat'(k)
            _temp_vector1.numRows = _z_mat.numRows;
            _temp_vector1.numCols = 1;
            _mat_status = Matrix_Subtract(
                    &_z_mat, &_temp_vector, &_temp_vector1
            ); // temp_vector1 = z(k) - H·xhat'(k)
            _temp_vector.numRows = _k_mat.numRows;
            _temp_vector.numCols = 1;
            _mat_status = Matrix_Multiply(
                    &_k_mat, &_temp_vector1, &_temp_vector
            ); // temp_vector = K(k)·(z(k) - H·xhat'(k))
            _mat_status = Matrix_Add(&_x_hat_minus_mat, &_temp_vector, &_x_hat_mat);
        }
        return this;
    }

    KalmanFilter *KalmanFilter::_pUpdate() {
        if (!_skip_p_update) {
            _temp_mat.numRows = _k_mat.numRows;
            _temp_mat.numCols = _h_mat.numCols;
            _temp_mat1.numRows = _temp_mat.numRows;
            _temp_mat1.numCols = _p_minus_mat.numCols;
            _mat_status = Matrix_Multiply(&_k_mat, &_h_mat, &_temp_mat);                 // temp_matrix = K(k)·H
            _mat_status = Matrix_Multiply(
                    &_temp_mat, &_p_minus_mat, &_temp_mat1
            ); // temp_matrix1 = K(k)·H·P'(k)
            _mat_status = Matrix_Subtract(&_p_minus_mat, &_temp_mat1, &_p_mat);
        }
        return this;
    }

    KalmanFilter *KalmanFilter::update() {
        _measure();

        // 1. xhat'(k)= A·xhat(k-1) + B·u
        _minusUpdate();

        // 2. P'(k) = A·P(k-1)·AT + Q
        _pMinusUpdate();

        if (_measurement_valid_num != 0 || _use_auto_adjustment == 0) {
            // 3. K(k) = P'(k)·HT / (H·P'(k)·HT + R)
            _setK();

            // 4. xhat(k) = xhat'(k) + K(k)·(z(k) - H·xhat'(k))
            _xHatUpdate();


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

        return this;
    }
}
