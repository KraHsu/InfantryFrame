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
 * @LastEditTime: 2024-01-17 14:03:36
 * Copyright (c) 2024 by KraHsu, All Rights Reserved.
 */
#pragma once

#include"main.h"

#include "infantry_event.h"
#include "infantry_thread.h"
#include "preprocessor.hpp"

#define DECLARE_STATE(index, name, state) BOOST_PP_IF(BOOST_PP_SUB(index, 1), BOOST_PP_COMMA, BOOST_PP_EMPTY)() state

#define DECLARE_FSM(NAME, ...) \
BOOST_PP_IF( \
    BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)), 30), \
    "#error : The maximum number of states is 30!", \
    BOOST_PP_EMPTY() \
)                              \
enum class NAME##States : uint8_t { \
        BOOST_PP_SEQ_FOR_EACH(DECLARE_STATE, NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)), \
        END \
    };                         \
constexpr uint8_t NAME##StatesSize = static_cast<uint8_t>(NAME##States::END); \
extern infantry::FSM NAME;     \
void NAME##Init()

#define DEFINE_FSM(NAME, ACTIONS, TRANSITIONS, DATA) \
void (*NAME##StateTransitions[NAME##StatesSize][NAME##StatesSize])(void *) = GENERATE_TWO_D_LIST TRANSITIONS; \
void (*NAME##StateActions[NAME##StatesSize])(void *) = GENERATE_ONE_D_LIST ACTIONS; \
infantry::FSM NAME{ \
    reinterpret_cast<void (**)(void *)>(NAME##StateTransitions), \
    reinterpret_cast<void (**)(void *)>(NAME##StateActions), \
    NAME##StatesSize, \
    DATA \
}; \
void NAME##Init()

#define GET_STATE(name, state) static_cast<uint8_t>(name##States::state)

namespace infantry {
    using v_vp_pp = void (**)(void *);

    class FiniteStateMachine {
    private:
        infantry::DynamicThread *_thread_ptr{nullptr}; /**< 状态机线程指针 */
        uint8_t _state{0}; /**< 状态机状态 */
        const uint8_t _state_max{0}; /**< 状态机状态最大值 */
        void (**_transitions)(void *){nullptr}; /**< 状态机状态切换函数表 */
        void (**_actions)(void *){nullptr}; /**< 状态机状态动作函数表 */
        void *_data{nullptr};
    public:
        friend void StateMachineMainLoop(void *fsm_ptr);

        /**
         * @brief 状态机构造函数
         * @param state_transitions 状态机状态切换函数表
         * @param actions 状态机状态动作函数表
         * @param state_max 状态机状态最大值
         * @param data 状态机数据指针
         */
        FiniteStateMachine(
                void (**state_transitions)(void *), void (**actions)(void *), uint8_t state_max, void *data = nullptr
        );

        FiniteStateMachine *init(
                const char *name = "fsm", rt_uint32_t stack_size = 1024, rt_uint8_t priority = 5, rt_uint32_t tick = 5
        );

        /**
         * @brief 设置状态机数据指针，该指针用于在状态机动作和传递函数中传递数据
         * @param data <void *> 状态机数据指针
         */
        FiniteStateMachine *setData(void *data);

        /**
         * @brief 切换状态机状态
         * @param next_state <uint8_t/FOO_STATES> 目标状态值
         */
        FiniteStateMachine *switchTo(uint8_t next_state);

        /**
         * @brief 设置状态动作
         * @param state <uint8_t> 状态值
         * @param func <void (*)(void *)> 动作函数指针，入参为数据指针
         * @see setData
         */
        FiniteStateMachine *setAction(uint8_t state, void (*func)(void *));

        /**
         * @brief 设置状态切换动作
         * @param from <uint8_t/FOO_STATES> 起始状态值
         * @param to <uint8_t/FOO_STATES> 目标状态值
         * @param func <void (*)(void *)> 动作函数指针，入参为 自身(StateMachine) 指针
         */
        FiniteStateMachine *setTransition(uint8_t from, uint8_t to, void (*func)(void *));

        /**
         * @brief 阻止状态切换
         * @param from <uint8_t/FOO_STATES> 起始状态值
         * @param to <uint8_t/FOO_STATES> 目标状态值
         */
        FiniteStateMachine *blockSwitch(uint8_t from, uint8_t to);

        /**
         * @brief 执行状态机
         */
        FiniteStateMachine *run();

        /**
         * @brief 执行状态机，一次
         */
        FiniteStateMachine *act();

        uint8_t getState();

        void *getDataPtr();

        ~FiniteStateMachine();
    };

    using FSM = FiniteStateMachine;
}
