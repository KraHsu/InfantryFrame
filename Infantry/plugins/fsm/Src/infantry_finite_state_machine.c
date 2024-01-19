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
 * @LastEditTime: 2024-01-17 14:06:24
 * Copyright (c) 2024 by KraHsu, All Rights Reserved.
 */
#include "infantry_finite_state_machine.h"

void StateMachineErrorHandler() {
    while (1) {
        // HACK: ...
    }
}

void StateMachineBlocking(void *) {
    while (1) {
        // HACK: ...
    }
}

void StateMachineBlockSwitch(StateMachine *state_machine_ptr, uint8_t from, uint8_t to) {
    StateMachineSetTransitionFunction(state_machine_ptr, from, to, StateMachineBlocking);
}

void StateMachineSwitchTo(StateMachine *state_machine_ptr, uint8_t next_state) {
    if (next_state > state_machine_ptr->stateMax) {
        StateMachineErrorHandler();
    }
    if (state_machine_ptr->stateTransitionFunctions[
                state_machine_ptr->state * state_machine_ptr->stateMax + next_state
        ] != NULL) {
        state_machine_ptr->stateTransitionFunctions[
                state_machine_ptr->state * state_machine_ptr->stateMax + next_state
        ](state_machine_ptr);
    }
    state_machine_ptr->state = next_state;
};

void StateMachineAct(StateMachine *state_machine_ptr) {
    if (state_machine_ptr->state > state_machine_ptr->stateMax) {
        StateMachineErrorHandler();
    }
    if (state_machine_ptr->actionFunctions[state_machine_ptr->state] == NULL) { return; }
    state_machine_ptr->actionFunctions[state_machine_ptr->state](state_machine_ptr);
}

void StateMachineSetTransitionFunction(
        StateMachine *state_machine_ptr, uint8_t from, uint8_t to, void (*func)(void *)) {
    if (state_machine_ptr != NULL && from < state_machine_ptr->stateMax && to < state_machine_ptr->stateMax) {
        state_machine_ptr->stateTransitionFunctions[from * state_machine_ptr->stateMax + to] = func;
    }
}

void StateMachineSetAction(StateMachine *state_machine_ptr, uint8_t state, void (*func)(void *)) {
    if (state_machine_ptr != NULL && state < state_machine_ptr->stateMax) {
        state_machine_ptr->actionFunctions[state] = func;
    }
}
