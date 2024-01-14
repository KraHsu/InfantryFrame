/**
 * @file Infantry/example/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 12/26/2023
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

        float angleFeedback = 0;
        switch (_state) {
            case encoderAngle:
                angleFeedback = rxData->_angles.limited_angle - 123.40f;
                break;
            case absoluteAngle:
                angleFeedback = _absolute_angle;
                break;
        }

        auto posOutput = _pid_pos->setReference(_reference).setFeedback(angleFeedback).calculate();
        auto spdOutput = _pid_spd->setReference(posOutput).setFeedback(rxData->_encoder.speed).calculate();
        auto output = _pid_cur->setReference(spdOutput).setFeedback(rxData->_encoder.filteredCurrent).calculate();

        txData->_current = int16_t(output);
//        logInfoWithTag("6020", "output is %d", txData->_current);
        return this;
    }

    PositionMotor *PositionMotor::setReference(float reference) {
        _reference = reference;
        return this;
    }

    PositionMotor *PositionMotor::setAbsoluteAngle(float absoluteAngle) {
        _absolute_angle = absoluteAngle;
        return this;
    }

    PositionMotor *PositionMotor::control() {
        transmit();
        return this;
    }

    PositionMotor *PositionMotor::setState(PositionMotor::State state) {
        _state = state;
        return this;
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

        auto spdOutput = _pid_spd->setReference(_reference).setFeedback(rxData->_encoder.speed).calculate();
        txData->_current = int16_t(
                _pid_cur->setReference(spdOutput).setFeedback(rxData->_encoder.filteredCurrent).calculate()
        );
//        logInfoWithTag("6020", "output is %d", txData->_current);
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
}

/* DjMotor */
namespace infantry {
    DjMotor::RxData *DjMotor::RxData::rxCallback(FDCAN_RxHeaderTypeDef header, std::uint8_t *buffer) {
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

    DjMotor::RxData::EncoderType DjMotor::RxData::getEncoderData() {
        return _encoder;
    }

    DjMotor::RxData::AngleType DjMotor::RxData::getAngleData() {
        return _angles;
    }

    DjMotor::TxData::TxData(FDCAN_HandleTypeDef *ph_fdcan, FDCAN_TxHeaderTypeDef header, uint8_t motor_id)
            : FdcanTxDataType(header, ph_fdcan, 8), _motor_id{motor_id} {}

    DjMotor::TxData *DjMotor::TxData::txHook() {
        if (_buffer_size == 0 || _buffer == nullptr) {
            FdcanErrorHandler("buffer empty");
        }
        _buffer[2 * _motor_id - 2] = uint8_t(_current >> 8);
        _buffer[2 * _motor_id - 1] = uint8_t(_current & 0xff);
        return this;
    }
}
