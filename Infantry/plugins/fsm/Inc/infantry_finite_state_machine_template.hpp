/*
 * _______________#########_______________________
 * ______________############_____________________
 * ______________#############____________________
 * _____________##__###########___________________
 * ____________###__######_#####__________________
 * ____________###_#######___####_________________
 * ___________###__##########_####________________
 * __________####__###########_####_______________
 * ________#####___###########__#####_____________
 * _______######___###_########___#####___________
 * _______#####___###___########___######_________
 * ______######___###__###########___######_______
 * _____######___####_##############__######______
 * ____#######__#####################_#######_____
 * ____#######__##############################____
 * ___#######__######_#################_#######___
 * ___#######__######_######_#########___######___
 * ___#######____##__######___######_____######___
 * ___#######________######____#####_____#####____
 * ____######________#####_____#####_____####_____
 * _____#####________####______#####_____###______
 * ______#####______;###________###______#________
 * ________##_______####________####______________
 *
 * @Date: 2024-01-17 15:32:59
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 15:54:39
 * Copyright (c) 2024 by KraHsu, All Rights Reserved.
 */
#pragma once

#include "main.h"
#include "infantry_def.h"

#ifdef __cplusplus

#include <map>
#include <functional>

namespace infantry {
    template<typename StateEnumT, typename DataT = NoData>
    class StateMachine {
        static_assert(std::is_enum<StateEnumT>::value, "Template parameter is not an enum type");
    protected:
        StateEnumT _state;

        std::map<std::pair<StateEnumT, StateEnumT>, std::function<void(DataT *)>> _transition_map;

        std::map<StateEnumT, std::function<void(DataT *)>> _action_map;

        DataT _data;
    public:
        explicit StateMachine<StateEnumT, DataT>(StateEnumT state) : _state(state) {};

        StateMachine<StateEnumT, DataT> *switchTo(StateEnumT next_state) {
            if (_transition_map.find(std::make_pair(_state, next_state)) != _transition_map.end()) {
                if (_transition_map[std::make_pair(_state, next_state)] != nullptr) {
                    _transition_map[std::make_pair(_state, next_state)](&_data);
                } else {
                    while(true) {
                        // TODO: error handler
                    }
                }
            }
            _state = next_state;
            return this;
        };

        StateMachine<StateEnumT, DataT> *act() {
            if (_action_map.find(_state) != _action_map.end()) {
                _action_map[_state](&_data);
            }
            return this;
        };

        StateMachine<StateEnumT, DataT> *blockSwitch(StateEnumT from, StateEnumT to) {
            _transition_map[std::make_pair(from, to)] = nullptr;
            return this;
        };

        template<typename CallableT>
        StateMachine<StateEnumT, DataT> *setAction(StateEnumT state, CallableT &&callable) {
            _action_map[state] = std::move(std::forward<std::function<void(DataT *)>>(callable));
            return this;
        };

        template<typename CallableT>
        StateMachine<StateEnumT, DataT> *setTransition(StateEnumT from, StateEnumT to, CallableT &&callable) {
            _transition_map[std::make_pair(from, to)] = std::move(
                    std::forward<std::function<void(DataT *)>>(callable));
            return this;
        };

        StateEnumT getState() const {
            return _state;
        };
    };
}

#endif
