/**
 * @file Infantry/plugins
 * @author CharlesHsu
 * @date 11/21/2023
 */

#pragma once

#include "SEGGER_RTT.h"
#include "plugins/logger/Inc/elog.h"

#define logAssertWithTag(tag, ...)     elog_assert(1, tag, __VA_ARGS__)
#define logErrorWithTag(tag, ...)     elog_error(2, tag, __VA_ARGS__)
#define logWarnWithTag(tag, ...)     elog_warn(3, tag, __VA_ARGS__)
#define logInfoWithTag(tag, ...)     elog_info(4, tag, __VA_ARGS__)
#define logDebugWithTag(tag, ...)     elog_debug(5, tag, __VA_ARGS__)
#define logVerboseWithTag(tag, ...)     elog_verbose(6, tag, __VA_ARGS__)

#define INF_ASSERT(EXPR) ELOG_ASSERT(EXPR)

#if !defined(LOG_TAG)
#define LOG_TAG          "NO_TAG"
#endif
#if !defined(LOG_LVL)
#define LOG_LVL          ELOG_LVL_VERBOSE
#endif
#if LOG_LVL >= ELOG_LVL_ASSERT
#define logAssert(...)       logAssertWithTag(LOG_TAG, __VA_ARGS__)
#else
#define logAssert(...)       ((void)0);
#endif
#if LOG_LVL >= ELOG_LVL_ERROR
#define logError(...)       logErrorWithTag(LOG_TAG, __VA_ARGS__)
#else
#define logError(...)       ((void)0);
#endif
#if LOG_LVL >= ELOG_LVL_WARN
#define logWarn(...)       logWarnWithTag(LOG_TAG, __VA_ARGS__)
#else
#define logWarn(...)       ((void)0);
#endif
#if LOG_LVL >= ELOG_LVL_INFO
#define logInfo(...)       logInfoWithTag(LOG_TAG, __VA_ARGS__)
#else
#define logInfo(...)       ((void)0);
#endif
#if LOG_LVL >= ELOG_LVL_DEBUG
#define logDebug(...)       logDebugWithTag(LOG_TAG, __VA_ARGS__)
#else
#define logDebug(...)       ((void)0);
#endif
#if LOG_LVL >= ELOG_LVL_VERBOSE
#define logVerbose(...)       logVerboseWithTag(LOG_TAG, __VA_ARGS__)
#else
#define logVerbose(...)       ((void)0);
#endif

#ifdef __cplusplus

namespace infantry {
    class LogOut {
    public:
        LogOut() = default;

        LogOut &operator<<(const char *str);

        LogOut &operator<<(char c);

        LogOut &operator<<(float f);

        LogOut &operator<<(int32_t d);
    };

    extern LogOut cout;

    extern const char *endl;
}

#endif
