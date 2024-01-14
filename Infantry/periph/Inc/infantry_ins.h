/**
 * @file Infantry/periph/Src
 * @author CharlesHsu
 * @date 12/22/2023
 */
#pragma once

#include "infantry_quaternion_extended_kalman_filter.h"

#define IMU InertialMeasurementUnit
#define INS InertialNavigationSystem

#define INS_X 0
#define INS_Y 1
#define INS_Z 2

namespace infantry {
    class IMU {
        using AccelStruct = struct {
            float x;
            float y;
            float z;
        };
        using SpeedStruct = struct {
            float yaw;
            float pitch;
            float row;
        };
    protected:
        /**
         * real data
         */
        AccelStruct _accel{};
        SpeedStruct _gyro{};
    public:
        friend class INS;

        /**
         * @brief You need to implement: Initialize the device
         * @return this
         */
        virtual IMU *init() = 0;

        /**
         * @brief You need to implement: Receive the data and decode it
         * @return this
         */
        virtual IMU *decodeData() = 0;

        /**
         * @brief You need to implement: Returns a Boolean value that represents whether the measurement data of the imu can be trusted
         * @return this
         */
        virtual bool isDependable() = 0;
    };

    class INS {
        using InsDataStruct = struct {
            uint8_t flag;
            float scale[3];
            float yaw;
            float pitch;
            float roll;
        };
    public:
        using AttitudeAngleIndex = enum {
            totalYaw = 0,
            pitch = 1,
            roll = 2
        };
    private:
        IMU *_imu{nullptr};

        QEKF *_qekf{nullptr};
//        QEKF_QEKFDataTypeDef *_qekf{nullptr};

        float _q[4]{0};
        float _gyro[3]{0}; //0:pitch, 1:roll, 2:yaw
        float _accel[3]{0};
        float _motion_accel_b[3]{0};
        float _motion_accel_n[3]{0};

        float _accel_low_pass_filter{0};

        float _xn[3]{0};
        float _yn[3]{0};
        float _zn[3]{0};

        float _atan_xz{0};
        float _atan_yz{0};

        float _roll{0};
        float _pitch{0};
        float _yaw{0};
        float _yaw_total_angle{0};

        InsDataStruct _param{0};

        float _update_dt{0};
        uint32_t _last_update_tick{0};
    public:
        /**
         * @brief Initialize INS
         * @param imu [class derived from the IMU]
         * @param qekf [QuaternionExtendedKalmanFilter]
         */
        INS(IMU *imu, QEKF *qekf);

        INS *init();

        INS *update();

        /**
         * @brief reserved.It is used to correct IMU installation error and scale factor error,
         *        i.e. the installation deviation of gyroscope axis and PTZ axis
         */
        INS *paramCorrection();

        INS *installCompAngle(const float correc_mat[3][3]);

        bool isDependable();

        void getAttitudeAngle(float attitudeAngleArray[3]) const;
    };

    void BodyFrameToEarthFrame(const float *vecBF, float *vecEF, const float *q);

    void EarthFrameToBodyFrame(const float *vecEF, float *vecBF, float *q);
} // infantry
