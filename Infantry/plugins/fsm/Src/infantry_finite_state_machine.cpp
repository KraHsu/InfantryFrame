/*
 *           佛曰:
 *                   写字楼里写字间，写字间里程序员；
 *                   程序人员写程序，又拿程序换酒钱。
 *                   酒醒只在网上坐，酒醉还来网下眠；
 *                   酒醉酒醒日复日，网上网下年复年。
 *                   但愿老死电脑间，不愿鞠躬老板前；
 *                   奔驰宝马贵者趣，公交自行程序员。
 *                   别人笑我忒疯癫，我笑自己命太贱；
 *                   不见满街漂亮妹，哪个归得程序员？
 *
 * @Date: 2024-01-17 02:33:41
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-23 12:09:43
 * Copyright (c) 2024 by KraHsu, All Rights Reserved.
 */
#include "infantry_finite_state_machine.h"

namespace infantry
{
    void StateMachineMainLoop(void *fsm_ptr)
    {
        auto fsmPtr = (FiniteStateMachine *)fsm_ptr;
        while (true)
        {
            if (fsmPtr->_actions[fsmPtr->_state] != nullptr)
            {
                fsmPtr->_actions[fsmPtr->_state](fsmPtr->_data);
            }
        }
    }

    [[noreturn]] void StateMachineErrorHandler(const char *msg)
    {
        logErrorWithTag("fsm", msg);
        while (true)
        {
            // TODO: ...
        }
    }

    static void StateMachineBlockingError(void *)
    {
        StateMachineErrorHandler("blocking error!");
    }

    FiniteStateMachine::FiniteStateMachine(
        void (**state_transitions)(void *), void (**actions)(void *), uint8_t state_max, void *data) : _transitions(state_transitions), _actions(actions), _state_max(state_max), _data(data) {}

    FiniteStateMachine *FiniteStateMachine::init(
        const char *name, rt_uint32_t stack_size, rt_uint8_t priority, rt_uint32_t tick)
    {
        _thread_ptr = new infantry::DynamicThread{
            StateMachineMainLoop, name, stack_size, priority, tick, (void *)(this)};
        return this;
    }

    FiniteStateMachine::~FiniteStateMachine()
    {
        delete _thread_ptr;
    }

    FiniteStateMachine *FiniteStateMachine::setData(void *data)
    {
        this->_data = data;
        return this;
    }

    FiniteStateMachine *FiniteStateMachine::switchTo(uint8_t next_state)
    {
        if (next_state < _state_max)
        {
            if (_transitions[_state * _state_max + next_state] != nullptr)
            {
                _transitions[_state * _state_max + next_state](_data);
            }
            _state = next_state;
        }
        return this;
    }

    FiniteStateMachine *FiniteStateMachine::setAction(uint8_t state, void (*func)(void *))
    {
        if (state < _state_max)
        {
            _transitions[state] = func;
        }
        return this;
    }

    FiniteStateMachine *FiniteStateMachine::setTransition(uint8_t from, uint8_t to, void (*func)(void *))
    {
        if (from < _state_max && to < _state_max)
        {
            _transitions[from * _state_max + to] = func;
        }
        return this;
    }

    FiniteStateMachine *FiniteStateMachine::run()
    {
        _thread_ptr->run();
        return this;
    }

    FiniteStateMachine *FiniteStateMachine::blockSwitch(uint8_t from, uint8_t to)
    {
        setTransition(from, to, StateMachineBlockingError);
        return this;
    }

    FiniteStateMachine *FiniteStateMachine::act()
    {
        if (_actions[_state] != nullptr)
        {
            _actions[_state](_data);
        }
        return this;
    }

    uint8_t FiniteStateMachine::getState()
    {
        return _state;
    }

    void *FiniteStateMachine::getDataPtr()
    {
        return _data;
    }
}
