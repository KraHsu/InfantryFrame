/**
 * @file Infantry/periph/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/22/2023
 */
#include <cmath>
#include "infantry_ins.h"
#include "arm_math.h"
#include "infantry_quaternion_extended_kalman_filter.h"
#include "infantry_dwt_lib.h"

constexpr float gravity[3] = {0, 0, 9.81f};
constexpr float install_comp[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}};

namespace infantry {
    INS::INS(IMU *imu, QEKF *qekf) : _imu{imu}, _qekf{qekf} {}

    INS *INS::init() {
        _imu->init();

        _param.scale[INS_X] = 1;
        _param.scale[INS_Y] = 1;
        _param.scale[INS_Z] = 1;
        _param.yaw = 0;
        _param.pitch = 0;
        _param.roll = 0;
        _param.flag = 1;

        _qekf->init();

        _accel_low_pass_filter = 0.0085f;
        return this;
    }

    INS *INS::update() {
        _update_dt = DWT_GetDeltaT(&_last_update_tick);
        _imu->decodeData();
        _accel[INS_X] = _imu->_accel.x;
        _accel[INS_Y] = _imu->_accel.y;
        _accel[INS_Z] = _imu->_accel.z;
        _gyro[INS_X] = _imu->_gyro.pitch;
        _gyro[INS_Y] = _imu->_gyro.row;
        _gyro[INS_Z] = _imu->_gyro.yaw;

        paramCorrection();

        // Calculate the angle between the gravity acceleration vector and the XY axis of the b system,
        // which can be used as a function extension.
        _atan_xz = -atan2f(_accel[INS_X], _accel[INS_Z]) * 180 / PI;
        _atan_yz = atan2f(_accel[INS_Y], _accel[INS_Z]) * 180 / PI;

        // Core function, EKF update quaternion
        _qekf->update(
                _gyro[INS_X], _gyro[INS_Y], _gyro[INS_Z], _accel[INS_X], _accel[INS_Y],
                _accel[INS_Z], _update_dt
        );

        memcpy(_q, _qekf->_q, sizeof(_qekf->_q));

        // The base vector of the airframe system is converted to the navigation coordinate system. In this example,
        // the inertial system is selected as the navigation system
        BodyFrameToEarthFrame(install_comp[INS_X], _xn, _q);
        BodyFrameToEarthFrame(install_comp[INS_Y], _yn, _q);
        BodyFrameToEarthFrame(install_comp[INS_Z], _zn, _q);

        // Convert the gravity from the navigation coordinate system n to the aircraft system b,
        // and then calculate the motion acceleration according to the accelerometer data
        float gravity_b[3];
        EarthFrameToBodyFrame(gravity, gravity_b, _q);
        for (uint8_t i = 0; i < 3; i++) {
            _motion_accel_b[i] =
                    (_accel[i] - gravity_b[i]) * _update_dt / (_accel_low_pass_filter + _update_dt) +
                    _motion_accel_b[i] * _accel_low_pass_filter / (_accel_low_pass_filter + _update_dt);
        }
        BodyFrameToEarthFrame(_motion_accel_b, _motion_accel_n, _q);

        installCompAngle(install_comp);

        return this;
    }

    INS *INS::paramCorrection() {
        static float lastYawOffset, lastPitchOffset, lastRollOffset;
        static float c_11, c_12, c_13, c_21, c_22, c_23, c_31, c_32, c_33;
        float cosPitch, cosYaw, cosRoll, sinPitch, sinYaw, sinRoll;

        if (fabsf(_param.yaw - lastYawOffset) > 0.001f ||
            fabsf(_param.pitch - lastPitchOffset) > 0.001f ||
            fabsf(_param.roll - lastRollOffset) > 0.001f || _param.flag) {
            cosYaw = arm_cos_f32(_param.yaw / 57.295779513f);
            cosPitch = arm_cos_f32(_param.pitch / 57.295779513f);
            cosRoll = arm_cos_f32(_param.roll / 57.295779513f);
            sinYaw = arm_sin_f32(_param.yaw / 57.295779513f);
            sinPitch = arm_sin_f32(_param.pitch / 57.295779513f);
            sinRoll = arm_sin_f32(_param.roll / 57.295779513f);

            // 1.yaw(alpha) 2.pitch(beta) 3.roll(gamma)
            c_11 = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
            c_12 = cosPitch * sinYaw;
            c_13 = cosYaw * sinRoll - cosRoll * sinYaw * sinPitch;
            c_21 = cosYaw * sinPitch * sinRoll - cosRoll * sinYaw;
            c_22 = cosYaw * cosPitch;
            c_23 = -sinYaw * sinRoll - cosYaw * cosRoll * sinPitch;
            c_31 = -cosPitch * sinRoll;
            c_32 = sinPitch;
            c_33 = cosPitch * cosRoll;
            _param.flag = 0;
        }
        float gyro_temp[3];
        for (uint8_t i = 0; i < 3; i++) {
            gyro_temp[i] = _gyro[i] * _param.scale[i];
        }

        _gyro[INS_X] = c_11 * gyro_temp[INS_X] +
                       c_12 * gyro_temp[INS_Y] +
                       c_13 * gyro_temp[INS_Z];
        _gyro[INS_Y] = c_21 * gyro_temp[INS_X] +
                       c_22 * gyro_temp[INS_Y] +
                       c_23 * gyro_temp[INS_Z];
        _gyro[INS_Z] = c_31 * gyro_temp[INS_X] +
                       c_32 * gyro_temp[INS_Y] +
                       c_33 * gyro_temp[INS_Z];

        float accel_temp[3];
        for (uint8_t i = 0; i < 3; i++) {
            accel_temp[i] = _accel[i];
        }

        _accel[INS_X] = c_11 * accel_temp[INS_X] +
                        c_12 * accel_temp[INS_Y] +
                        c_13 * accel_temp[INS_Z];
        _accel[INS_Y] = c_21 * accel_temp[INS_X] +
                        c_22 * accel_temp[INS_Y] +
                        c_23 * accel_temp[INS_Z];
        _accel[INS_Z] = c_31 * accel_temp[INS_X] +
                        c_32 * accel_temp[INS_Y] +
                        c_33 * accel_temp[INS_Z];

        lastYawOffset = _param.yaw;
        lastPitchOffset = _param.pitch;
        lastRollOffset = _param.roll;
        return this;
    }

    INS *INS::installCompAngle(const float (*correc_mat)[3]) {
        _pitch = _qekf->_pitch * correc_mat[0][0] + _qekf->_roll * correc_mat[0][1];
        _roll = _qekf->_pitch * correc_mat[1][0] + _qekf->_roll * correc_mat[1][1];
        _yaw = _qekf->_yaw;
        _yaw_total_angle = _qekf->_yaw_total_angle;
        return this;
    }

    bool InertialNavigationSystem::isDependable() {
        return _imu != nullptr && _imu->isDependable();
    }

    void INS::getAttitudeAngle(float attitudeAngleArray[3]) const {
        attitudeAngleArray[AttitudeAngleIndex::totalYaw] = _yaw_total_angle;
        attitudeAngleArray[AttitudeAngleIndex::pitch] = _pitch;
        attitudeAngleArray[AttitudeAngleIndex::roll] = _roll;
    }

    void BodyFrameToEarthFrame(const float *vecBF, float *vecEF, const float *q) {
        vecEF[0] = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecBF[0] +
                           (q[1] * q[2] - q[0] * q[3]) * vecBF[1] +
                           (q[1] * q[3] + q[0] * q[2]) * vecBF[2]);

        vecEF[1] = 2.0f * ((q[1] * q[2] + q[0] * q[3]) * vecBF[0] +
                           (0.5f - q[1] * q[1] - q[3] * q[3]) * vecBF[1] +
                           (q[2] * q[3] - q[0] * q[1]) * vecBF[2]);

        vecEF[2] = 2.0f * ((q[1] * q[3] - q[0] * q[2]) * vecBF[0] +
                           (q[2] * q[3] + q[0] * q[1]) * vecBF[1] +
                           (0.5f - q[1] * q[1] - q[2] * q[2]) * vecBF[2]);
    }

    void EarthFrameToBodyFrame(const float *vecEF, float *vecBF, float *q) {
        vecBF[0] = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecEF[0] +
                           (q[1] * q[2] + q[0] * q[3]) * vecEF[1] +
                           (q[1] * q[3] - q[0] * q[2]) * vecEF[2]);

        vecBF[1] = 2.0f * ((q[1] * q[2] - q[0] * q[3]) * vecEF[0] +
                           (0.5f - q[1] * q[1] - q[3] * q[3]) * vecEF[1] +
                           (q[2] * q[3] + q[0] * q[1]) * vecEF[2]);

        vecBF[2] = 2.0f * ((q[1] * q[3] + q[0] * q[2]) * vecEF[0] +
                           (q[2] * q[3] - q[0] * q[1]) * vecEF[1] +
                           (0.5f - q[1] * q[1] - q[2] * q[2]) * vecEF[2]);
    }
} // infantry
