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

namespace infantry {
    INS::INS(IMU *imu, QEKF *qekf) : _imu{imu}, _qekf{qekf} {}

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
