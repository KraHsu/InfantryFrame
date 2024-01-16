/**
 * @file Infantry/example/Src
 * @author CharlesHsu
 * @date 12/23/2023
 */

#include <cmath>
#include "bmi088.h"
#include "infantry_dwt_lib.h"
#include "infantry_math.h"

float buff2float(uint8_t *buff) {
    uint8_t tempBuff[4];
    memcpy(tempBuff, buff, 4);
    return *((float *) tempBuff);
}

void ui322buff(uint32_t u, uint8_t *buff) {
    uint8_t tempBuff[4];
    *((uint32_t *) tempBuff) = u;
    memcpy(buff, tempBuff, 4);
}

Bmi088::Bmi088(infantry::SpiDevice *spi, infantry::GpioDevice *accel_start, infantry::GpioDevice *gyro_start)
        : _spi{spi}, _accel_start{accel_start}, _gyro_start{gyro_start} {
    resetBMI088Data();
}

Bmi088 *Bmi088::resetBMI088Data() {
    _accel.x = 0;
    _accel.y = 0;
    _accel.z = 0;
    _gyro.pitch = 0;
    _gyro.row = 0;
    _gyro.yaw = 0;
    _temperature = 0;
    _cali_offset = 1;
    _update_dt = 0;
    _last_update_tick = 0;
    return this;
}

Bmi088 *Bmi088::init() {
    resetBMI088Data();

    uint8_t error = BMI088_NO_ERROR;
    do {
        _calibrate = 1;
        _accel_sen = BMI088_ACCEL_6G_SEN;
        _gyro_sen = BMI088_GYRO_2000_SEN;

        error = (Bmi088AccelTest(this) != BMI088_NO_ERROR) ?
                BMI088_SELF_TEST_ACCEL_ERROR : Bmi088AccelInit(this);

        error = (Bmi088GyroTest(this) != BMI088_NO_ERROR) ?
                BMI088_SELF_TEST_GYRO_ERROR : Bmi088GyroInit(this);

        _calibrate == 1 ? setOffset() : getOffset();

        _state = error != BMI088_NO_ERROR ? BMI088_STATE_LOST : BMI088_STATE_CONNECTED;
    } while (error);
    return this;
}

Bmi088 *Bmi088::accelWriteSingleReg(uint8_t reg, uint8_t data) {
    uint8_t rxData;
    _accel_start->reset();
    _spi->swapOneByteData(reg, &rxData)->swapOneByteData(data, &rxData);
    _accel_start->set();
    return this;
}

Bmi088 *Bmi088::accelReadSingleReg(uint8_t reg, uint8_t *data_ptr) {
    uint8_t rxData;
    _accel_start->reset();
    _spi->swapOneByteData(reg | 0x80, &rxData)->swapOneByteData(0x55, &rxData)->swapOneByteData(0x55, data_ptr);
    _accel_start->set();
    return this;
}

Bmi088 *Bmi088::gyroWriteSingleReg(uint8_t reg, uint8_t data) {
    uint8_t rxData;
    _gyro_start->reset();
    _spi->swapOneByteData(reg, &rxData)->swapOneByteData(data, &rxData);
    _gyro_start->set();
    return this;
}

Bmi088 *Bmi088::gyroReadSingleReg(uint8_t reg, uint8_t *data_ptr) {
    uint8_t rxData;
    _gyro_start->reset();
    _spi->swapOneByteData(reg | 0x80, &rxData)->swapOneByteData(0x55, data_ptr);
    _gyro_start->set();
    return this;
}

Bmi088 *Bmi088::accelReadMultiReg(uint8_t reg, uint8_t *data_ptr, uint8_t len) {
    _accel_start->reset();
    _spi->swapOneByteData(reg | 0x80, data_ptr)->swapOneByteData(reg | 0x80, data_ptr)
        ->readMultiReg(0x55, data_ptr, len);
    _accel_start->set();
    return this;
}

Bmi088 *Bmi088::gyroReadMultiReg(uint8_t reg, uint8_t *data_ptr, uint8_t len) {
    _gyro_start->reset();
    _spi->swapOneByteData(reg | 0x80, data_ptr)->readMultiReg(0x55, data_ptr, len);
    _gyro_start->set();
    return this;
}

uint8_t Bmi088AccelInit(Bmi088 *bmi088) {
    uint8_t res = 0;

    // check commiunication
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    // accel software reset
    bmi088->accelWriteSingleReg(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
    DWT_Delayms(BMI088_LONG_DELAY_TIME);

    // check commiunication is normal after reset
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    // check the "who am I"
    if (res != BMI088_ACC_CHIP_ID_VALUE) { return BMI088_NO_SENSOR; }

    // set accel sonsor config and check
    for (auto &write_reg_num: write_BMI088_accel_reg_data_error) {

        bmi088->accelWriteSingleReg(
                write_reg_num[0],
                write_reg_num[1]
        );
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        bmi088->accelReadSingleReg(write_reg_num[0], &res);
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != write_reg_num[1]) {
            return write_reg_num[2];
        }

    }
    return BMI088_NO_ERROR;
}

uint8_t Bmi088GyroInit(Bmi088 *bmi088) {
    uint8_t res = 0;

    // check commiunication
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    // reset the gyro sensor
    bmi088->gyroWriteSingleReg(BMI088_GYRO_SOFTRESET, BMI088_GYRO_SOFTRESET_VALUE);
    DWT_Delayms(BMI088_LONG_DELAY_TIME);
    // check commiunication is normal after reset
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    // check the "who am I"
    if (res != BMI088_GYRO_CHIP_ID_VALUE) { return BMI088_NO_SENSOR; }


    // set gyro sonsor config and check
    for (int write_reg_num = 0; write_reg_num < BMI088_WRITE_GYRO_REG_NUM; write_reg_num++) {

        bmi088->gyroWriteSingleReg(
                write_BMI088_gyro_reg_data_error[write_reg_num][0],
                write_BMI088_gyro_reg_data_error[write_reg_num][1]
        );
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        bmi088->gyroReadSingleReg(write_BMI088_gyro_reg_data_error[write_reg_num][0], &res);
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != write_BMI088_gyro_reg_data_error[write_reg_num][1]) {
            return write_BMI088_gyro_reg_data_error[write_reg_num][2];
        }

    }

    return BMI088_NO_ERROR;
}

uint8_t Bmi088AccelTest(Bmi088 *bmi088) {
    uint8_t res = 0;
    int16_t self_test_accel[2][3];
    uint8_t buff[6] = {0, 0, 0, 0, 0, 0};

    // check commiunication is normal
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    // reset  bmi088 accel sensor and wait for > 50ms
    bmi088->accelWriteSingleReg(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
    DWT_Delayms(BMI088_LONG_DELAY_TIME);

    // check commiunication is normal
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    if (res != BMI088_ACC_CHIP_ID_VALUE) { return BMI088_NO_SENSOR; }

    // set the accel register
    for (int write_reg_num = 0; write_reg_num < 4; write_reg_num++) {

        bmi088->accelWriteSingleReg(
                write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num][0],
                write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num][1]
        );
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        bmi088->accelReadSingleReg(write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num][0], &res);
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num][1]) {
            return write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num][2];
        }

        // accel conf and accel range  . the two register set need wait for > 50ms
        DWT_Delayms(BMI088_LONG_DELAY_TIME);
    }

    // self test include postive and negative
    for (int write_reg_num = 0; write_reg_num < 2; write_reg_num++) {

        bmi088->accelWriteSingleReg(
                write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][0],
                write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][1]
        );
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        bmi088->accelReadSingleReg(write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][0], &res);
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][1]) {
            return write_BMI088_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][2];
        }

        // accel conf and accel range  . the two register set need wait for > 50ms
        DWT_Delayms(BMI088_LONG_DELAY_TIME);

        // read response accel
        bmi088->accelReadMultiReg(BMI088_ACCEL_XOUT_L, buff, 6);

        self_test_accel[write_reg_num][0] = (int16_t) ((buff[1]) << 8) | buff[0];
        self_test_accel[write_reg_num][1] = (int16_t) ((buff[3]) << 8) | buff[2];
        self_test_accel[write_reg_num][2] = (int16_t) ((buff[5]) << 8) | buff[4];
    }

    // set self test off
    bmi088->accelWriteSingleReg(BMI088_ACC_SELF_TEST, BMI088_ACC_SELF_TEST_OFF);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->accelReadSingleReg(BMI088_ACC_SELF_TEST, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    if (res != (BMI088_ACC_SELF_TEST_OFF)) { return BMI088_ACC_SELF_TEST_ERROR; }


    // reset the accel sensor
    bmi088->accelWriteSingleReg(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
    DWT_Delayms(BMI088_LONG_DELAY_TIME);

    if ((self_test_accel[0][0] - self_test_accel[1][0] < 1365) ||
        (self_test_accel[0][1] - self_test_accel[1][1] < 1365) ||
        (self_test_accel[0][2] - self_test_accel[1][2] < 680)) {
        return BMI088_SELF_TEST_ACCEL_ERROR;
    }


    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->accelReadSingleReg(BMI088_ACC_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    return BMI088_NO_ERROR;
}

uint8_t Bmi088GyroTest(Bmi088 *bmi088) {
    uint8_t res = 0;
    uint8_t retry = 0;

    // check commiunication is normal
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    // reset the gyro sensor
    bmi088->gyroWriteSingleReg(BMI088_GYRO_SOFTRESET, BMI088_GYRO_SOFTRESET_VALUE);
    DWT_Delayms(BMI088_LONG_DELAY_TIME);

    // check commiunication is normal after reset
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
    bmi088->gyroReadSingleReg(BMI088_GYRO_CHIP_ID, &res);
    DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);

    bmi088->gyroWriteSingleReg(BMI088_GYRO_SELF_TEST, BMI088_GYRO_TRIG_BIST);
    DWT_Delayms(BMI088_LONG_DELAY_TIME);

    do {
        bmi088->gyroReadSingleReg(BMI088_GYRO_SELF_TEST, &res);
        DWT_Delayms(BMI088_COM_WAIT_SENSOR_TIME);
        retry++;
    } while (!(res & BMI088_GYRO_BIST_RDY) && retry < 10);

    if (retry == 10) { return BMI088_SELF_TEST_GYRO_ERROR; }


    if (res & BMI088_GYRO_BIST_FAIL) { return BMI088_SELF_TEST_GYRO_ERROR; }


    return BMI088_NO_ERROR;
}

Bmi088 *Bmi088::setOffset() {
    float gyroMax[3], gyroMin[3];
    float gNormTemp, gNormMax, gNormMin, gNormDiff, gyroDiff[3];
    uint8_t buff[8] = {0, 0, 0, 0, 0, 0};
    static uint16_t CaliTimes = 6000;
    int16_t caliCount = 0;
    float start_time = DWT_GetTimeline_s();

    do {
        if (DWT_GetTimeline_s() - start_time > 10) {
            _gyro_offset[0] = BMI088_GxOFFSET;
            _gyro_offset[1] = BMI088_GyOFFSET;
            _gyro_offset[2] = BMI088_GzOFFSET;
            _g_norm = BMI088_gNORM;
            //LED_SetLEDState(&LED_RED, LED_FLASHING);
            break;
        }
        //LED_SetLEDState(&LED_GREEN, LED_FLASHING);
        DWT_Delay(0.005);
        _g_norm = 0;
        _gyro_offset[0] = 0;
        _gyro_offset[1] = 0;
        _gyro_offset[2] = 0;

        for (uint16_t i = 0; i < CaliTimes; i++) {
            accelReadMultiReg(BMI088_ACCEL_XOUT_L, buff, 6);
            _accel.x = ((int16_t) ((buff[1]) << 8) | buff[0]) * _accel_sen;
            _accel.y = ((int16_t) ((buff[3]) << 8) | buff[2]) * _accel_sen;
            _accel.z = ((int16_t) ((buff[5]) << 8) | buff[4]) * _accel_sen;

            gNormTemp = sqrtf(
                    _accel.x * _accel.x +
                    _accel.y * _accel.y +
                    _accel.z * _accel.z
            );
            _g_norm += gNormTemp;

            gyroReadMultiReg(BMI088_GYRO_CHIP_ID, buff, 8);
            if (buff[0] == BMI088_GYRO_CHIP_ID_VALUE) {
                _gyro.pitch = ((int16_t) ((buff[3]) << 8) | buff[2]) * _gyro_sen;
                _gyro.row = ((int16_t) ((buff[5]) << 8) | buff[4]) * _gyro_sen;
                _gyro.yaw = ((int16_t) ((buff[7]) << 8) | buff[6]) * _gyro_sen;
                _gyro_offset[0] += _gyro.pitch;
                _gyro_offset[1] += _gyro.row;
                _gyro_offset[2] += _gyro.yaw;
            }

            if (i == 0) {
                gNormMax = gNormTemp;
                gNormMin = gNormTemp;
                gyroMax[0] = _gyro.pitch;
                gyroMin[0] = _gyro.pitch;
                gyroMax[1] = _gyro.row;
                gyroMin[1] = _gyro.row;
                gyroMax[2] = _gyro.yaw;
                gyroMin[2] = _gyro.yaw;
            } else {
                GetMaxandMinRange(gNormTemp, gNormMax, gNormMin);
                GetMaxandMinRange(_gyro.pitch, gyroMax[0], gyroMin[0]);
                GetMaxandMinRange(_gyro.row, gyroMax[1], gyroMin[1]);
                GetMaxandMinRange(_gyro.yaw, gyroMax[2], gyroMin[2]);
            }

            gNormDiff = gNormMax - gNormMin;
            for (uint8_t j = 0; j < 3; j++) {
                gyroDiff[j] = gyroMax[j] - gyroMin[j];
            }
            if (gNormDiff > 0.7f || gyroDiff[0] > 0.15f || gyroDiff[1] > 0.15f || gyroDiff[2] > 0.15f) {
                break;
            }
            DWT_Delay(0.0005);
        }

        _g_norm /= (float) CaliTimes;
        for (float &i: _gyro_offset) {
            i /= (float) CaliTimes;
        }

        caliCount++;
    } while (gNormDiff > 0.7f ||
             fabsf(_g_norm - 9.8f) > 0.5f ||
             gyroDiff[0] > 0.15f ||
             gyroDiff[1] > 0.15f ||
             gyroDiff[2] > 0.15f ||
             fabsf(_gyro_offset[0]) > 0.01f ||
             fabsf(_gyro_offset[1]) > 0.01f ||
             fabsf(_gyro_offset[2]) > 0.01f);

    _accel_scale = 9.81f / _g_norm;
    return this;
}

Bmi088 *Bmi088::getOffset() {
    uint32_t offset_buf[4];
    float offset[4];
    //Flash_ReadData(BMI088_OFFSET_ADDRESS, offset_buf, 4);
    for (int i = 0; i < 4; i++) {
        uint8_t temp[4];
        ui322buff(offset_buf[i], temp);
        offset[i] = buff2float(temp);
    }

    if (offset[0] > 0.015f ||
        offset[1] > 0.015f ||
        offset[2] > 0.015f ||
        offset[3] > 11.0f ||
        offset[3] < 9.5f) {
        _gyro_offset[0] = BMI088_GxOFFSET;
        _gyro_offset[1] = BMI088_GyOFFSET;
        _gyro_offset[2] = BMI088_GzOFFSET;
        _g_norm = BMI088_gNORM;
        _accel_scale = 9.81f / _g_norm;
    } else {
        for (int i = 0; i < 3; i++) {
            _gyro_offset[i] = offset[i];
        }

        _g_norm = offset[3];
        _accel_scale = 9.81f / _g_norm;
    }
    return this;
}

Bmi088 *Bmi088::decodeData() {
    uint8_t buff[8] = {0, 0, 0, 0, 0, 0};
    int16_t raw_temp;

    _state = BMI088_STATE_PENDING;
    _update_dt = DWT_GetDeltaT(&_last_update_tick);

    accelReadMultiReg(BMI088_ACCEL_XOUT_L, buff, 6);

    _accel.x = ((int16_t) ((buff[1]) << 8) | buff[0]) * _accel_sen * _accel_scale;
    _accel.y = ((int16_t) ((buff[3]) << 8) | buff[2]) * _accel_sen * _accel_scale;
    _accel.z = ((int16_t) ((buff[5]) << 8) | buff[4]) * _accel_sen * _accel_scale;

    gyroReadMultiReg(BMI088_GYRO_CHIP_ID, buff, 8);
    if (buff[0] == BMI088_GYRO_CHIP_ID_VALUE) {
        if (_cali_offset) {
            _gyro.pitch =
                    ((int16_t) ((buff[3]) << 8) | buff[2]) * _gyro_sen - _gyro_offset[0];
            _gyro.row =
                    ((int16_t) ((buff[5]) << 8) | buff[4]) * _gyro_sen - _gyro_offset[1];
            _gyro.yaw =
                    ((int16_t) ((buff[7]) << 8) | buff[6]) * _gyro_sen - _gyro_offset[2];
        } else {
            _gyro.pitch = ((int16_t) ((buff[3]) << 8) | buff[2]) * _gyro_sen;
            _gyro.row = ((int16_t) ((buff[5]) << 8) | buff[4]) * _gyro_sen;
            _gyro.yaw = ((int16_t) ((buff[7]) << 8) | buff[6]) * _gyro_sen;
        }
    }
    accelReadMultiReg(BMI088_TEMP_M, buff, 2);

    raw_temp = (int16_t) ((buff[0] << 3) | (buff[1] >> 5));
    if (raw_temp > 1023) {
        raw_temp -= 2048;
    }

    _temperature = raw_temp * BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET;
    _state = BMI088_STATE_CONNECTED;
    return this;
}

bool Bmi088::isDependable() {
    return _g_norm < 10 && _g_norm > 9;
}
