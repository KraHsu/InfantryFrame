/**
 * @file Infantry/frame/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 11/24/2023
 */
#include "infantry_mutex.h"
#include "infantry_log.h"

namespace infantry {
    StaticMutex::~StaticMutex() {
        rt_mutex_detach(&_mutex);
    }

    rt_err_t StaticMutex::init(const char *name, rt_uint8_t flag) {
        return rt_mutex_init(&_mutex, name, flag);
    }

    rt_err_t StaticMutex::take() {
        return rt_mutex_take(&_mutex, RT_WAITING_NO);
    }

    rt_err_t StaticMutex::take(rt_int32_t time) {
        return rt_mutex_take(&_mutex, time);
    }

    rt_err_t StaticMutex::release() {
        return rt_mutex_release(&_mutex);
    }

    DynamicMutex::DynamicMutex(const char *name, rt_uint8_t flag) {
        _mutex_t = rt_mutex_create(name, flag);
        INF_ASSERT(_mutex_t != RT_NULL);
    }

    DynamicMutex::~DynamicMutex() {
        auto res = rt_mutex_delete(_mutex_t);
        INF_ASSERT(res == RT_EOK);
    }

    rt_err_t DynamicMutex::take(rt_int32_t time = RT_WAITING_NO) {
        return rt_mutex_take(_mutex_t, time);
    }

    rt_err_t DynamicMutex::release() {
        return rt_mutex_release(_mutex_t);
    }
} // infantry
