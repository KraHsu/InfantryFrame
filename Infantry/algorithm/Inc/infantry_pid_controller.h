/**
 * @file Infantry/algorithm
 * @author CharlesHsu
 * @date 11/12/2023
 */
#pragma once

#include "algorithm/Inc/infantry_low_pass_filter.h"
#include "algorithm/Inc/infantry_math.h"

namespace infantry {
    class PIDController {
    public:
        using ModeEnum = enum {
            POSITION = 0u,
            DELTA = 1u
        };
        using InitialType = struct {
            float Kp;
            float Ki;
            float Kd;

            float SumMax;
            float OutputMax;

            float KdFilterParam;
            float DeltaFilterParam;

            float Kf1Param;
            float Kf2Param;

            float Kf1FilterParam;
            float Kf2FilterParam;
        };
    private:
        ModeEnum _mode{POSITION};

        // PID
        float _kp{};  // pid param
        float _ki{};
        float _kd{};

        float _sum_max{};     // sum limit
        float _output_max{};  // out limit

        // Feedforward
        float _kf_1{};  // feedforward param
        float _kf_2{};

        float _reference{};
        float _feedback{};
        float _err[3]{};
        float _err_lim{};     // Integral anti-windup
        float _err_feed_forward[3]{};  // Feedforard
        float _out_feed_forward{};     // Feedforard out put
        float _sum{};
        float _output{};

        LowPassFilter _kd_fil;
        LowPassFilter _delta_fil;

        LowPassFilter _kf1_fil;  //低通滤波
        LowPassFilter _kf2_fil;
    public:
        PIDController(
                float kp, float ki, float kd, float sum_max, float output_max, float kd_fil_param,
                float delta_fil_param, float kf_1_param, float kf_2_param, float kf1_fil_param, float kf2_fil_param,
                ModeEnum pid_mode
        );

        PIDController(InitialType initial_struct, ModeEnum pid_mode);

        ~PIDController() = default;

        float getReference() const;

        PIDController &setReference(float ref);

        PIDController &addReference(float inc);

        float getFeedback() const;

        PIDController &setFeedback(float fdb);

        float calculate();

        PIDController &clear();
    };
} // infantry
