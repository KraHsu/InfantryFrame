/**
 * @file Infantry/plugins
 * @author CharlesHsu
 * @date 11/21/2023
 */
#include "rtthread.h"
#include "rthw.h"

#include "infantry_def.h"
#include "plugins/logger/Inc/infantry_log.h"

#include "SEGGER_RTT.h"

[[noreturn]] static void elog_user_assert_hook(const char *ex, const char *func, size_t line) {

#ifdef ELOG_ASYNC_OUTPUT_ENABLE
    elog_async_enabled(false);
#endif

    elog_output_lock_enabled(false);
    logAssertWithTag("logger", "(%s) has assert failed at %s:%ld.\n", ex, func, line);
    while (true) {
        // assert
    };
}

static rt_err_t exception_hook(void *context) {
    struct exception_stack_frame {
        rt_uint32_t r0;
        rt_uint32_t r1;
        rt_uint32_t r2;
        rt_uint32_t r3;
        rt_uint32_t r12;
        rt_uint32_t lr;
        rt_uint32_t pc;
        rt_uint32_t psr;
    };
    auto *exception_stack = (struct exception_stack_frame *) context;

    /* disable logger output lock */
    elog_output_lock_enabled(false);

    logErrorWithTag("hw_fault", "psr: 0x%08x", exception_stack->psr);
    logErrorWithTag("hw_fault", " pc: 0x%08x", exception_stack->pc);
    logErrorWithTag("hw_fault", " lr: 0x%08x", exception_stack->lr);
    logErrorWithTag("hw_fault", "r12: 0x%08x", exception_stack->r12);
    logErrorWithTag("hw_fault", "r03: 0x%08x", exception_stack->r3);
    logErrorWithTag("hw_fault", "r02: 0x%08x", exception_stack->r2);
    logErrorWithTag("hw_fault", "r01: 0x%08x", exception_stack->r1);
    logErrorWithTag("hw_fault", "r00: 0x%08x", exception_stack->r0);
    logErrorWithTag("hw_fault", "hard fault on thread: %s", rt_thread_self()->name);
    while (true) {};

    return RT_EOK;
}

EXPORT_BOARD_SETUP(init_log) {
    SEGGER_RTT_Init();
    SEGGER_RTT_SetTerminal(0);
    auto level = rt_hw_interrupt_disable();
    if (elog_init() == ELOG_NO_ERR) {
        /* 设置日志格式 */
        elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_FUNC);
        elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_FUNC);
        elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_FUNC);
        elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_FUNC);
        elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_FUNC);
        elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_FUNC);
        /* 设置断言回调 */
        elog_assert_set_hook(elog_user_assert_hook);
        /* 启动 */
        elog_start();
        /* 设置 exception 回调 */
        rt_hw_exception_install(exception_hook);
    } else {
        while (true) {
            // error handler
        }
    }
    rt_hw_interrupt_enable(level);
}

const char *getHALResponseString(uint8_t response) {
    switch (response) {
        case 0x00u:
            return "HAL_OK";
        case 0x01u:
            return "HAL_ERROR";
        case 0x02u:
            return "HAL_BUSY";
        case 0x03u:
            return "HAL_TIMEOUT";
        default:
            return "UNKNOWN";
    }
}

#ifdef __cplusplus
namespace infantry {
    LogOut cout{};

    const char *endl = "\n";

    LogOut &LogOut::operator<<(const char *str) {
        print(str);
        return *this;
    }

    LogOut &LogOut::operator<<(char c) {
        print("%c", c);
        return *this;
    }

    LogOut &LogOut::operator<<(float f) {
        print("%f", f);
        return *this;
    }

    LogOut &LogOut::operator<<(int32_t d) {
        print("%d", d);
        return *this;
    }
}

#endif
