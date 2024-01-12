/**
 * @file Infantry/frame/Src
 * @author CharlesHsu
 * @date 11/24/2023
 */
#pragma once

#include "rtthread.h"

namespace infantry {
    class StaticEvent {
    private:
        rt_event _event;
    public:
        StaticEvent() = default;

        ~StaticEvent();

        rt_err_t init(const char *name, rt_uint8_t flag);

        rt_err_t send(rt_uint32_t set);

        rt_err_t receive(rt_uint32_t set, rt_uint8_t option, rt_int32_t timeout, rt_uint32_t *recved);
    };

    class DynamicEvent {
    private:
        rt_event_t _event_t;
    public:
        DynamicEvent(const char *name, rt_uint8_t flag);

        ~DynamicEvent();

        rt_err_t send(rt_uint32_t set);

        rt_err_t receive(rt_uint32_t set, rt_uint8_t option, rt_int32_t timeout, rt_uint32_t *recved);
    };
}
