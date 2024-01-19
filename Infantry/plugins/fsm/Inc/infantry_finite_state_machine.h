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

#include "main.h"
#include "infantry_preprocessor.h"

#define DECLARE_STATE(index, contex, arg) PP_IF(index, PP_COMMA, PP_EMPTY)() contex##_##arg
/**
 * @brief DECLARE_FSM(NAME, ...) --> 声明并初始化状态机
 * @param NAME 状态机名称，将会生成 NAME_STATES, NAME
 * @param ... 状态列表 list ，将会生成 NAME_##list[0], NAME_##list[1], ...
 */
#define DECLARE_FSM(NAME, ...)                                                  \
typedef enum {                                                                  \
    NAME##_##BEGIN = 0,                                                            \
    PP_FOR_EACH(DECLARE_STATE, NAME, __VA_ARGS__, END)                          \
} NAME##_##STATES;                                                                 \
void (*NAME##StateTransitionFunctions[NAME##_##END][NAME##_##END])();\
void (*NAME##StateActionFunctions[NAME##_##END])();                      \
StateMachine NAME{                                                \
    .state = 0,                                                                 \
    .stateTransitionFunctions = (void (**)(void *))NAME##StateTransitionFunctions,\
    .stateMax = NAME##_##END,                                         \
    .actionFunctions = (void(**)(void *))NAME##StateActionFunctions,                \
    .data = NULL                                                              \
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 状态机结构体
 */
typedef struct {
    uint8_t state; /**< 状态机状态 */

    const uint8_t stateMax; /**< 状态机状态最大值 */

    void (**stateTransitionFunctions)(void *); /**< 状态机状态切换函数指针数组 */

    void (**actionFunctions)(void *); /**< 状态机状态动作函数指针数组 */

    void *data; /**< 状态机数据指针 <void *> */
} StateMachine;

/**
 * @brief 切换状态机状态
 * @param state_machine_ptr [out] <StateMachine *> 实例指针
 * @param next_state <uint8_t/FOO_STATES> 目标状态值
 */
void StateMachineSwitchTo(StateMachine *state_machine_ptr, uint8_t next_state);

/**
 * @brief 阻止状态切换
 * @param state_machine_ptr [out] <StateMachine *> 实例指针
 * @param from <uint8_t/FOO_STATES> 起始状态值
 * @param to <uint8_t/FOO_STATES> 目标状态值
 */
void StateMachineBlockSwitch(StateMachine *state_machine_ptr, uint8_t from, uint8_t to);

/**
 * @brief 执行状态机动作
 * @param state_machine_ptr [in] <StateMachine *> 实例指针
 */
void StateMachineAct(StateMachine *state_machine_ptr);

/**
 * @brief 设置状态切换动作
 * @param state_machine_ptr [out] <StateMachine *> 实例指针
 * @param from <uint8_t/FOO_STATES> 起始状态值
 * @param to <uint8_t/FOO_STATES> 目标状态值
 * @param func <void (*)(void *)> 动作函数指针，入参为 自身(StateMachine) 指针
 */
void StateMachineSetTransitionFunction(
        StateMachine *state_machine_ptr, uint8_t from, uint8_t to, void (*func)(void *)
);

/**
 * @brief 设置状态动作
 * @param state_machine_ptr [out] <StateMachine *> 实例指针
 * @param state <uint8_t/FOO_STATES> 状态值
 * @param func <void (*)(void *)> 动作函数指针，入参为 自身(StateMachine) 指针
 */
void StateMachineSetAction(StateMachine *state_machine_ptr, uint8_t state, void (*func)(void *));

#ifdef __cplusplus
}
#endif
