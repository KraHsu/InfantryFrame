/**
 * @file Infantry/frame/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 11/24/2023
 */
#include "infantry_event.h"
#include "infantry_log.h"

namespace infantry {
    infantry::StaticEvent::~StaticEvent() {
        rt_event_detach(&_event);
    }

    rt_err_t StaticEvent::init(const char *name, rt_uint8_t flag) {
        return rt_event_init(&_event, name, flag);
    }

    rt_err_t StaticEvent::send(rt_uint32_t set) {
        return rt_event_send(&_event, set);
    }

    rt_err_t StaticEvent::receive(rt_uint32_t set, rt_uint8_t option, rt_int32_t timeout, rt_uint32_t *recved) {
        return rt_event_recv(&_event, set, option, timeout, recved);
    }

    DynamicEvent::DynamicEvent(const char *name, rt_uint8_t flag) {
        _event_t = rt_event_create(name, flag);
        INF_ASSERT(_event_t != RT_NULL);
    }

    DynamicEvent::~DynamicEvent() {
        auto res = rt_event_delete(_event_t);
        INF_ASSERT(res == RT_EOK);
    }

    rt_err_t DynamicEvent::send(rt_uint32_t set) {
        return rt_event_send(_event_t, set);
    }

    rt_err_t DynamicEvent::receive(rt_uint32_t set, rt_uint8_t option, rt_int32_t timeout, rt_uint32_t *recved) {
        return rt_event_recv(_event_t, set, option, timeout, recved);
    }
}
