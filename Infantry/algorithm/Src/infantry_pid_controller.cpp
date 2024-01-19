/*
 * 
 * 　　┏┓　　　┏┓+ +
 * 　┏┛┻━━━┛┻┓ + +
 * 　┃　　　　　　　┃ 　
 * 　┃　　　━　　　┃ ++ + + +
 *  ████━████ ┃+
 * 　┃　　　　　　　┃ +
 * 　┃　　　┻　　　┃
 * 　┃　　　　　　　┃ + +
 * 　┗━┓　　　┏━┛
 * 　　　┃　　　┃　　　　　　　　　　　
 * 　　　┃　　　┃ + + + +
 * 　　　┃　　　┃
 * 　　　┃　　　┃ +  神兽保佑
 * 　　　┃　　　┃    代码无bug　　
 * 　　　┃　　　┃　　+　　　　　　　　　
 * 　　　┃　 　　┗━━━┓ + +
 * 　　　┃ 　　　　　　　┣┓
 * 　　　┃ 　　　　　　　┏┛
 * 　　　┗┓┓┏━┳┓┏┛ + + + +
 * 　　　　┃┫┫　┃┫┫
 * 　　　　┗┻┛　┗┻┛+ + + +
 * 
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:52:49
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#include "algorithm/Inc/infantry_pid_controller.h"

namespace infantry {

    PIDController::PIDController(
            float kp, float ki, float kd, float sum_max, float output_max, float kd_fil_param, float delta_fil_param,
            float kf_1_param, float kf_2_param, float kf1_fil_param, float kf2_fil_param,
            PIDController::ModeEnum pid_mode
    ) : _mode{pid_mode}, _kp{kp}, _ki{ki}, _kd{kd}, _sum_max{sum_max}, _output_max{output_max}, _kd_fil(kd_fil_param),
        _delta_fil(delta_fil_param), _kf_1{kf_1_param}, _kf_2{kf_2_param}, _kf1_fil(kf1_fil_param),
        _kf2_fil(kf2_fil_param) {}

    PIDController::PIDController(PIDController::InitialType initial_struct, PIDController::ModeEnum pid_mode)
            : PIDController(
            initial_struct.Kp, initial_struct.Ki, initial_struct.Kd, initial_struct.SumMax, initial_struct.OutputMax,
            initial_struct.KdFilterParam, initial_struct.DeltaFilterParam, initial_struct.Kf1Param,
            initial_struct.Kf2Param, initial_struct.Kf1FilterParam, initial_struct.Kf2FilterParam, pid_mode
    ) {}

    float PIDController::getReference() const {
        return _reference;
    }

    PIDController *PIDController::setReference(float ref) {
        this->_reference = ref;
        return this;
    }

    PIDController *PIDController::addReference(float inc) {
        _reference += inc;
        return this;
    }

    float PIDController::getFeedback() const {
        return _feedback;
    }

    PIDController *PIDController::setFeedback(float fdb) {
        _feedback = fdb;
        return this;
    }

    float PIDController::calculate() {
        if (_mode == POSITION) {
            float dError, Error, ref_dError, ref_ddError;

            // Calculate the differential value
            Error = _reference - _feedback;
            _err[2] = _err[1];
            _err[1] = _err[0];
            _err[0] = Error;

            dError = MathDifferential(_err, 1);

            _err_feed_forward[2] = _err_feed_forward[1];
            _err_feed_forward[1] = _err_feed_forward[0];
            _err_feed_forward[0] = _reference;

            ref_dError = MathDifferential(_err_feed_forward, 1);
            ref_ddError = MathDifferential(_err_feed_forward, 2);

            // Calculate the integral and integra anti-windup
            if (_kp == 0) {
                _sum = _sum + Error;
            } else {
                _sum = _sum + Error + _err_lim / _kp;
            }

            // Integral limiting
            LimitMax(_sum, _sum_max);

            // Calculation results kf1_filter
            _out_feed_forward = _kf1_fil.calculate(_kf_1 * ref_dError) + _kf2_fil.calculate(_kf_2 * ref_ddError);
            //out_fdf = kf_1 * ref_dError +kf_2 * ref_ddError;
            _output = _kp * Error + _ki * _sum + _kd * _kd_fil.calculate(dError) + _out_feed_forward;
        } else if (_mode == DELTA) {
            float dError, ddError, Error, ref_dError, ref_ddError;

            // Calculate the difference
            Error = _reference - _feedback;
            _err[2] = _err[1];
            _err[1] = _err[0];
            _err[0] = Error;

            dError = _delta_fil.calculate(MathDifferential(_err, 1));
            ddError = MathDifferential(_err, 2);

            _err_feed_forward[2] = _err_feed_forward[1];
            _err_feed_forward[1] = _err_feed_forward[0];
            _err_feed_forward[0] = _reference;

            ref_dError = MathDifferential(_err_feed_forward, 1);
            ref_ddError = MathDifferential(_err_feed_forward, 2);

            // Calculate the integral and integral anti-windup
            if (_kp == 0) {
                _sum = Error;
            } else {
                _sum = Error + _err_lim / _kp;
            }

            // Integral limiting
            LimitMax(_sum, _sum_max);

            // Calculation results kf1_filter
            _out_feed_forward = _kf1_fil.calculate(_kf_1 * ref_dError) + _kf2_fil.calculate(_kf_2 * ref_ddError);
            _output += _kp * dError + _ki * _sum + _kd * _kd_fil.calculate(ddError);
            _output += _out_feed_forward;
        }

        // Output limiting
        float temp_limit = _output;
        LimitMax(_output, _output_max);
        _err_lim = _output - temp_limit;
        return _output;
    }

    PIDController *PIDController::clear() {
        _reference = 0;
        _feedback = 0;
        _err[0] = 0;
        _err[1] = 0;
        _err[2] = 0;
        _err_lim = 0;
        _err_feed_forward[0] = 0;
        _err_feed_forward[1] = 0;
        _err_feed_forward[2] = 0;
        _out_feed_forward = 0;
        _sum = 0;
        _output = 0;
        return this;
    }
} // infantry
