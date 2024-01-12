/**
 * @file Infantry/example/Src
 * @author CharlesHsu
 * @date 12/23/2023
 */

#pragma once

#include "BMI088reg.h"
#include "infantry_spi_device.h"
#include "infantry_gpio_device.h"
#include "infantry_ins.h"


namespace infantry {
    class Bmi088 : public IMU {
        using StateEnum = enum {
            BMI088_STATE_NULL = 0,
            BMI088_STATE_CONNECTED = 1,
            BMI088_STATE_LOST = 2,
            BMI088_STATE_ERROR = 3,
            BMI088_STATE_PENDING = 4
        };
    private:
        SpiDevice *_spi{nullptr};
        GpioDevice *_accel_start{nullptr};
        GpioDevice *_gyro_start{nullptr};

        StateEnum _state{BMI088_STATE_NULL};

        float _d_accel[3]{0};
        float _d_gyro[3]{0};

        float _gyro_offset[3]{0};
        float _accel_offset[3]{0};
        float _g_norm{0};
        float _accel_scale{0};
        float _sensor_time{0};
        float _temperature{0};

        uint8_t _cali_offset{0};
        float _accel_sen{0};
        float _gyro_sen{0};
        uint8_t _calibrate{0};

        float _update_dt{0};
        uint32_t _last_update_tick{0};
    public:
        Bmi088(SpiDevice *spi, GpioDevice *accel_start, GpioDevice *gyro_start);

        Bmi088 *init() override;

        Bmi088 *decodeData() override;

        bool isDependable() override;

        Bmi088 *resetBMI088Data();

        Bmi088 *accelWriteSingleReg(uint8_t reg, uint8_t data);

        Bmi088 *accelReadSingleReg(uint8_t reg, uint8_t *data_ptr);

        Bmi088 *accelReadMultiReg(uint8_t reg, uint8_t *data_ptr, uint8_t len);

        Bmi088 *gyroWriteSingleReg(uint8_t reg, uint8_t data);

        Bmi088 *gyroReadSingleReg(uint8_t reg, uint8_t *data_ptr);

        Bmi088 *gyroReadMultiReg(uint8_t reg, uint8_t *data_ptr, uint8_t len);

        Bmi088 *setOffset();

        Bmi088 *getOffset();

        friend uint8_t Bmi088AccelInit(Bmi088 *bmi088);

        friend uint8_t Bmi088GyroInit(Bmi088 *bmi088);

        friend uint8_t Bmi088AccelTest(Bmi088 *bmi088);

        friend uint8_t Bmi088GyroTest(Bmi088 *bmi088);
    };
}
