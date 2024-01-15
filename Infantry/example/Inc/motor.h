/**
 * @file Infantry/example/Src
 * @author CharlesHsu
 * @date 12/26/2023
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
            int8_t has_init;  // Initial flag of the plucking wheel
            float init_offset;
            float limited_angle;
            float consequent_angle;
        };
    public:
        class RxData;

        class TxData;

        using FdcanDevice::FdcanDevice;

        AngleType *getAnglePtr();

        EncoderType *getEncoderPtr();
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

        TxData(FDCAN_HandleTypeDef *ph_fdcan, FDCAN_TxHeaderTypeDef header, uint8_t motor_id);

        TxData *txHook() override;
    };
}

/* PositionMotor */
namespace infantry {
    class PositionMotor final : public DjMotor {
    public:
        using State = enum {
            encoderAngle = 0,
            absoluteAngle = 1
        };
    private:
        PIDController *_pid_cur, *_pid_spd, *_pid_pos;
        float *_absolute_angle{nullptr};
        State _state{encoderAngle};
    public:
        class RxData final : public DjMotor::RxData {
        public:
            friend class PositionMotor;

            using DjMotor::RxData::RxData;
        };

        class TxData final : public DjMotor::TxData {
        public:
            friend class PositionMotor;

            using DjMotor::TxData::TxData;
        };

        PositionMotor(
                RxData *rx_data, TxData *tx_data, PIDController *current_pid, PIDController *speed_pid,
                PIDController *position_pid
        );

        PositionMotor *setReference(float reference) override;

        PositionMotor *setAbsoluteAngleGetter(float *absoluteAnglePtr);

        PositionMotor *setState(State state);

        PositionMotor *update() override;

        PositionMotor *control() override;
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
