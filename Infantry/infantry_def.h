/**
 * @file Infantry
 * @author CharlesHsu
 * @date 11/19/2023
 */
#pragma once

#include <cstdint>

namespace infantry {
    class NoData {
    };
};

#define delay(x) rt_thread_mdelay(x)
#define sleep(x) DWT_Delayms(x)
#define sleep_us(x) DWT_Delayus(x)
#define print rt_kprintf

#define EnterCritical() rt_enter_critical()
#define ExitCritical()  rt_exit_critical()

#define STRINGIFY(x) #x
#define AssertNoData(DataType) _Static_assert( \
    !std::is_same<DataType, NoData>::value, \
    #DataType " should not be NoData" \
)

/** 实现静态线程的动态导出 **/
using StaticCallType = struct {
    void (*entry)(void *parameter);

    const char *name;
    struct rt_thread *thread;
    void *stack_start;
    uint32_t stack_size;
    uint8_t priority;
    uint32_t tick;
};

#define EXPORT_STATIC_THREAD(fn, stack_size, priority, tick) \
    void fn(void *);                               \
    struct rt_thread fn##thread;                                \
    char fn##stack[stack_size];                                 \
    static StaticCallType static_call_##fn  __attribute__((used, section(".static_call.1"))) = {fn, #fn, &fn##thread, fn##stack, sizeof fn##stack, priority, tick}; \
    void fn(void *param)

using SetupCallType = struct {
    void (*entry)();
};

#define EXPORT_SETUP(fn, level) \
    void fn(); \
    static SetupCallType setup_call_##fn __attribute__((used, section(".setup_call." level))) = {fn}; \
    void fn() \

#define EXPORT_BOARD_SETUP(fn) EXPORT_SETUP(fn, "1")
#define EXPORT_PREV_SETUP(fn) EXPORT_SETUP(fn, "2")
#define EXPORT_DEVICE_SETUP(fn) EXPORT_SETUP(fn, "3")
#define EXPORT_PLUGIN_SETUP(fn) EXPORT_SETUP(fn, "4")
#define EXPORT_ENV_SETUP(fn) EXPORT_SETUP(fn, "5")
/** ** **/

/** 导出命令 **/
#define EXPORT_CMD(fn, desc) \
    static void fn(int argc, char **argv); \
    FINSH_FUNCTION_EXPORT_CMD(fn, __cmd_##fn, desc) \
    static void fn(int argc, char **argv)
/** ** **/

#define CRITICAL_SECTION(code) rt_enter_critical(); code rt_exit_critical();
#define CRITICAL_SECTION_BEGIN rt_enter_critical(); {
#define CRITICAL_SECTION_END } rt_exit_critical();

#define INF_SYNC_FIFO RT_IPC_FLAG_FIFO
#define INF_SYNC_PRIO RT_IPC_FLAG_PRIO
#define INF_SYNC_FOREVER RT_WAITING_FOREVER
#define INF_EVENT_OR RT_EVENT_FLAG_OR
#define INF_EVENT_AND RT_EVENT_FLAG_AND
#define INF_EVENT_CLEAR RT_EVENT_FLAG_CLEAR

#define INF_TASK_EXECUTE_LOOP (0)
#define INF_TASK_EXECUTE_ONCE (1)
#define INF_TASK_EXECUTE_N(n) (n)

#define Import extern
