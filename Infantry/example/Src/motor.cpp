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
 * @LastEditTime: 2024-01-17 01:54:08
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#include "motor.h"

/* PositionMotor */
namespace infantry {
    PositionMotor::PositionMotor(
            PositionMotor::RxData *rx_data, PositionMotor::TxData *tx_data, PIDController *current_pid,
            PIDController *speed_pid, PIDController *position_pid
    ) : DjMotor(rx_data, tx_data), _pid_cur(current_pid), _pid_spd(speed_pid), _pid_pos(position_pid) {}

    PositionMotor *PositionMotor::update() {
        auto *txData = (PositionMotor::TxData *) (_tx_data);
        auto *rxData = (PositionMotor::RxData *) (_rx_data);

        if (_pid_fdb_ptr[PositionPid] != nullptr) {
            _pid_pos->setFeedback(*_pid_fdb_ptr[PositionPid])
                    ->setReference(
                            ClampWithLR(_reference - *_pid_fdb_ptr[PositionPid], -60.0f, 60.0f)
                            + *_pid_fdb_ptr[PositionPid]
                    );
        } else {
            _pid_pos->setFeedback(rxData->_angles.consequent_angle)
                    ->setReference(
                            ClampWithLR(_reference - rxData->_angles.consequent_angle, -60.0f, 60.0f)
                            + rxData->_angles.consequent_angle
                    );
        }

        if (_pid_fdb_ptr[SpeedPid] != nullptr) {
            _pid_spd->setFeedback(*_pid_fdb_ptr[SpeedPid]);
        } else {
            _pid_spd->setFeedback(rxData->_encoder.speed);
        }

        if (_pid_fdb_ptr[CurrentPid] != nullptr) {
            _pid_cur->setFeedback(*_pid_fdb_ptr[CurrentPid]);
        } else {
            _pid_cur->setFeedback(rxData->_encoder.filteredCurrent);
        }

        auto posOutput = _pid_pos->calculate();
        auto spdOutput = _pid_spd->setReference(posOutput)->calculate();
        auto output = _pid_cur->setReference(spdOutput)->calculate();

        txData->_current = int16_t(output);
        return this;
    }

    PositionMotor *PositionMotor::setReference(float reference) {
        _reference = reference;
        return this;
    }

    PositionMotor *PositionMotor::control() {
        transmit();
        return this;
    }

    PositionMotor *PositionMotor::setFdbPtr(const float *currentPtr, const float *speedPtr, const float *positionPtr) {
        _pid_fdb_ptr[CurrentPid] = currentPtr;
        _pid_fdb_ptr[SpeedPid] = speedPtr;
        _pid_fdb_ptr[PositionPid] = positionPtr;
        return this;
    }

    DjMotor::AngleType *DjMotor::getAnglePtr() {
        return &(((DjMotor::RxData *) _rx_data)->_angles);
    }

    DjMotor::EncoderType *DjMotor::getEncoderPtr() {
        return &(((DjMotor::RxData *) _rx_data)->_encoder);
    }
}

/* SpeedMotor */
namespace infantry {
    SpeedMotor::SpeedMotor(
            SpeedMotor::RxData *rx_data, SpeedMotor::TxData *tx_data, PIDController *current_pid,
            PIDController *speed_pid
    ) : DjMotor(rx_data, tx_data), _pid_spd{speed_pid}, _pid_cur{current_pid} {}

    SpeedMotor *SpeedMotor::update() {
        auto *rxData = (SpeedMotor::RxData *) (_rx_data);
        auto *txData = (SpeedMotor::TxData *) (_tx_data);

        auto spdOutput = _pid_spd->setReference(_reference)->setFeedback(rxData->_encoder.speed)->calculate();
        txData->_current = int16_t(
                _pid_cur->setReference(spdOutput)->setFeedback(rxData->_encoder.filteredCurrent)->calculate()
        );
//        logInfoWithTag("6020", "outp->t is %d", txData->_current);
        return this;
    }

    SpeedMotor *SpeedMotor::setReference(float reference) {
        _reference = reference;
        return this;
    }

    SpeedMotor *SpeedMotor::control() {
        transmit();
        return this;
    }

    SpeedMotor::TxData *SpeedMotor::TxData::init(uint8_t *buffer) {
        _buffer = buffer;
        return this;
    }

    SpeedMotorGroup::SpeedMotorGroup(FDCAN_TxHeaderTypeDef header, FDCAN_HandleTypeDef *ph_fdcan)
            : _header{header}, _ph_fdcan{ph_fdcan} {}

    SpeedMotorGroup *SpeedMotorGroup::init(
            SpeedMotor *motor0, SpeedMotor *motor1, SpeedMotor *motor2, SpeedMotor *motor3
    ) {
        _motor[0] = motor0;
        _motor[1] = motor1;
        _motor[2] = motor2;
        _motor[3] = motor3;
        for (auto &motor: _motor) {
            ((SpeedMotor::TxData *) motor->_tx_data)->init((uint8_t *) _output);
            motor->_rx_data->init();
        }
        return this;
    }

    SpeedMotorGroup *SpeedMotorGroup::update() {
        for (auto &motor: _motor) {
            motor->update();
        }
        return this;
    }


    SpeedMotorGroup *SpeedMotorGroup::registerRxCallback() {
        for (auto &motor: _motor) {
            motor->registerRxCallback();
        }
        return this;
    }

    SpeedMotorGroup *SpeedMotorGroup::serReference(
            float motor0_ref, float motor1_ref, float motor2_ref, float motor3_ref
    ) {
        _motor[0]->setReference(motor0_ref);
        _motor[1]->setReference(motor1_ref);
        _motor[2]->setReference(motor2_ref);
        _motor[3]->setReference(motor3_ref);
        return this;
    }

    SpeedMotorGroup *SpeedMotorGroup::control() {
        for (auto &motor: _motor) {
            ((SpeedMotor::TxData *) motor->_tx_data)->txHook();
        }
        if (HAL_FDCAN_AddMessageToTxFifoQ(_ph_fdcan, &_header, _output) != HAL_OK) {
            logErrorWithTag("fdcan", "Data sending failure");
        }
        return this;
    }

    SpeedMotorGroup *SpeedMotorGroup::setVelocity(float normal, float tangential, float angular) {
        _motor[0]->setReference((normal + tangential + 0.5f * angular) * 19.2f);
        _motor[1]->setReference((-normal + tangential + 0.5f * angular) * 19.2f);
        _motor[2]->setReference((-normal - tangential + 0.5f * angular) * 19.2f);
        _motor[3]->setReference((normal - tangential + 0.5f * angular) * 19.2f);
        return this;
    }
}

/* DjMotor */
namespace infantry {
    DjMotor::RxData *DjMotor::RxData::rxCallback(FDCAN_RxHeaderTypeDef header, std::uint8_t *buffer) {
        if (header.Identifier != stdID) {
            return this;
        }
        _angles.last_angle = _encoder.angle;
        _encoder.angle = int16_t(buffer[0] << 8 | buffer[1]);
        _encoder.speed = int16_t(buffer[2] << 8 | buffer[3]);
        _encoder.current = int16_t(buffer[4] << 8 | buffer[5]);
        _encoder.temp = int8_t(buffer[6]);

        _encoder.filteredCurrent = _feedback_filter.calculate((float) _encoder.current);

        // Calculate angle difference and number of cycles
        int diff = _encoder.angle - _angles.last_angle;  // The increase of mechanical angle is positive
        if (diff < -5500) {                                               // Make a positive turn
            _angles.round_count++;
        } else if (diff > 5500) {  // Turn around in the opposite direction
            _angles.round_count--;
        }

        // Calculate continuous angle
        _angles.consequent_angle = (float) _angles.round_count * 360.0f + (float) _encoder.angle / 8192.0f * 360.0f;
        _angles.limited_angle = (float) _encoder.angle / 8192.0f * 360.0f;

        return this;
    }

    DjMotor::TxData::TxData(FDCAN_HandleTypeDef *ph_fdcan, FDCAN_TxHeaderTypeDef header, uint8_t motor_id)
            : FdcanTxDataType(ph_fdcan, header, 8), _motor_id{motor_id} {}

    DjMotor::TxData *DjMotor::TxData::txHook() {
        if (_buffer_size == 0 || _buffer == nullptr) {
            FdcanErrorHandler("buffer empty");
        }
        _buffer[2 * _motor_id - 2] = uint8_t(_current >> 8);
        _buffer[2 * _motor_id - 1] = uint8_t(_current & 0xff);
        return this;
    }
}
