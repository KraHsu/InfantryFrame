/**
 * @file Infantry/frame/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 11/22/2023
 */
#include "frame/Inc/infantry_sem.h"
#include "plugins/logger/Inc/infantry_log.h"

namespace infantry {
    StaticSem::~StaticSem() {
        INF_ASSERT(rt_sem_detach(&_semaphore) == RT_EOK);
    }

    rt_err_t StaticSem::init(const char *name, rt_uint32_t value, rt_uint8_t flag) {
        return rt_sem_init(&_semaphore, name, value, flag);
    }

    rt_err_t StaticSem::take() {
        return rt_sem_trytake(&_semaphore);
    }

    rt_err_t StaticSem::take(rt_int32_t time) {
        return rt_sem_take(&_semaphore, time);
    }

    rt_err_t StaticSem::release() {
        return rt_sem_release(&_semaphore);
    }

    const rt_uint16_t &StaticSem::getValue() const {
        return _semaphore.value;
    }

    DynamicSem::DynamicSem(const char *name, rt_uint32_t value, rt_uint8_t flag) {
        _semphore_t = rt_sem_create(name, value, flag);
        INF_ASSERT(_semphore_t != RT_NULL);
    }

    DynamicSem::~DynamicSem() {
        auto res = rt_sem_delete(_semphore_t);
        INF_ASSERT(res == RT_EOK);
    }

    rt_err_t DynamicSem::take() {
        return rt_sem_trytake(_semphore_t);
    }

    rt_err_t DynamicSem::take(rt_int32_t time) {
        return rt_sem_take(_semphore_t, time);
    }

    rt_err_t DynamicSem::release() {
        return rt_sem_release(_semphore_t);
    }

    const rt_uint16_t &DynamicSem::getValue() {
        return _semphore_t->value;
    }
}
