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

/* DjMotor */
namespace infantry {
    DjMotor::AngleType *DjMotor::getAnglePtr() {
        return &(((DjMotor::RxData *) _rx_data)->_angles);
    }

    DjMotor::EncoderType *DjMotor::getEncoderPtr() {
        return &(((DjMotor::RxData *) _rx_data)->_encoder);
    }

    DjMotor *DjMotor::setOutput(int16_t output) {
        ((DjMotor::TxData *) _tx_data)->_current = output;
        return this;
    };
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

        // Calculate angle difference and number of cyclesj
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

    DjMotor::TxData::TxData(FDCAN_HandleTypeDef *ph_fdcan, uint32_t can_id, uint8_t motor_id, uint8_t *buffer)
            : FdcanTxDataType(
            ph_fdcan, {
                    .Identifier=can_id, .IdType=FDCAN_STANDARD_ID, .TxFrameType=FDCAN_DATA_FRAME,
                    .DataLength=FDCAN_DLC_BYTES_8, .ErrorStateIndicator=FDCAN_ESI_ACTIVE, .BitRateSwitch = FDCAN_BRS_OFF,
                    .FDFormat=FDCAN_CLASSIC_CAN, .TxEventFifoControl = FDCAN_NO_TX_EVENTS, .MessageMarker = 0
            }, 8, buffer
    ), _motor_id{motor_id} {}

    DjMotor::TxData *DjMotor::TxData::txHook() {
        if (_buffer_size == 0 || _buffer == nullptr) {
            FdcanErrorHandler("buffer empty");
        }
        _buffer[2 * _motor_id - 2] = uint8_t(_current >> 8);
        _buffer[2 * _motor_id - 1] = uint8_t(_current & 0xff);
        return this;
    }
}
