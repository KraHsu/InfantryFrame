/**
 * @file Infantry/frame/Src
 * @author CharlesHsu
 * @date 11/24/2023
 */
#pragma once

#include "rtthread.h"

namespace infantry {

    class StaticMutex {
    private:
        rt_mutex _mutex;
    public:
        StaticMutex() = default;

        ~StaticMutex();

        rt_err_t init(const char *name, rt_uint8_t flag);

        rt_err_t take();

        rt_err_t take(rt_int32_t time);

        rt_err_t release();
    };

    class DynamicMutex {
    private:
        rt_mutex_t _mutex_t;
    public:
        DynamicMutex(const char *name, rt_uint8_t flag);

        ~DynamicMutex();

        rt_err_t take(rt_int32_t time);

        rt_err_t release();
    };

} // infantry
