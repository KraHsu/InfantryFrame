/**
 * @file Infantry/frame/Src
 * @author CharlesHsu
 * @date 11/22/2023
 */
#pragma once

#include "rtthread.h"

namespace infantry {
    class StaticSem {
    private:
        rt_semaphore _semaphore{};
    public:
        StaticSem() = default;

        ~StaticSem();

        rt_err_t init(const char *name, rt_uint32_t value, rt_uint8_t flag);

        rt_err_t take();

        rt_err_t take(rt_int32_t time);

        const rt_uint16_t &getValue() const;

        rt_err_t release();
    };

    class DynamicSem {
    private:
        rt_sem_t _semphore_t;
    public:
        DynamicSem(const char *name, rt_uint32_t value, rt_uint8_t flag);

        ~DynamicSem();

        rt_err_t take();

        rt_err_t take(rt_int32_t time);

        const rt_uint16_t &getValue();

        rt_err_t release();
    };
}
