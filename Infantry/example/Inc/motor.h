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
 * @LastEditTime: 2024-01-17 01:54:31
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include "device/Inc/infantry_fdcan_device.h"
#include "algorithm/Inc/infantry_pid_controller.h"
#include "infantry_ins.h"

/* Motor */
namespace infantry {
    class Motor {
    protected:
        float _reference{0};
    public:
        virtual Motor *setReference(float reference) = 0;

        virtual Motor *update() = 0;

        virtual Motor *control() = 0;
    };
}

/* DjMotor */
namespace infantry {
    class DjMotor : public FdcanDevice, public Motor {
    public:
        using EncoderType = struct {
            int16_t angle;
            int16_t speed;
            int16_t current;
            float filteredCurrent;
            int8_t temp;
        };
        using AngleType = struct {
            int16_t last_angle;
            int16_t round_count;
            float init_offset;
            float limited_angle;
            float consequent_angle;
        };
    public:
        class RxData;

        class TxData;

        using FdcanDevice::FdcanDevice;

        Motor *setReference(float reference) override {
            return this;
        };

        Motor *update() override {
            return this;
        };

        Motor *control() override {
            transmit();
            return this;
        };

        AngleType *getAnglePtr();

        EncoderType *getEncoderPtr();

        DjMotor *setOutput(int16_t output);
    };

    class DjMotor::RxData : public FdcanRxDataType {
    protected:
        EncoderType _encoder{};
        AngleType _angles{};
        LowPassFilter _feedback_filter{-1};
    public:
        friend class DjMotor;

        using FdcanRxDataType::FdcanRxDataType;

        RxData *rxCallback(FDCAN_RxHeaderTypeDef header, std::uint8_t *buffer) override;
    };

    class DjMotor::TxData : public FdcanTxDataType {
    protected:
        uint8_t _motor_id;
        int16_t _current{0};
    public:
        friend class DjMotor;

        TxData(FDCAN_HandleTypeDef *ph_fdcan, uint32_t can_id, uint8_t motor_id, uint8_t* buffer = nullptr);

        TxData *txHook() override;
    };
}

/* SpeedMotor */
namespace infantry {
    class SpeedMotor final : public DjMotor {
    private:
        PIDController *_pid_cur, *_pid_spd;
    public:
        friend class SpeedMotorGroup;

        class RxData final : public DjMotor::RxData {
        public:
            friend class SpeedMotor;

            using DjMotor::RxData::RxData;
        };

        class TxData final : public DjMotor::TxData {
        public:
            friend class SpeedMotor;

            using DjMotor::TxData::TxData;

            TxData *init(uint8_t *buffer);
        };

        SpeedMotor(RxData *rx_data, TxData *tx_data, PIDController *current_pid, PIDController *speed_pid);

        SpeedMotor *setReference(float reference) override;

        SpeedMotor *update() override;

        SpeedMotor *control() override;
    };

    class SpeedMotorGroup final {
    private:
        uint8_t _output[8]{0};
        SpeedMotor *_motor[4]{nullptr};

        FDCAN_TxHeaderTypeDef _header;
        FDCAN_HandleTypeDef *_ph_fdcan;
    public:
        SpeedMotorGroup(FDCAN_TxHeaderTypeDef header, FDCAN_HandleTypeDef *ph_fdcan);

        SpeedMotorGroup *init(SpeedMotor *motor0, SpeedMotor *motor1, SpeedMotor *motor2, SpeedMotor *motor3);

        SpeedMotorGroup *serReference(float motor0_ref, float motor1_ref, float motor2_ref, float motor3_ref);

        SpeedMotorGroup *setVelocity(float normal, float tangential, float angular);

        SpeedMotorGroup *update();

        SpeedMotorGroup *control();

        SpeedMotorGroup *registerRxCallback();
    };
}
