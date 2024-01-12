/**
 * @file Infantry/frame/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 11/28/2023
 */
#include "infantry_messagequeue.h"
#include "infantry_log.h"

namespace infantry {
    StaticMessageQueue::StaticMessageQueue(void *msg_pool) : _msg_pool(msg_pool), _mq() {}

    StaticMessageQueue::~StaticMessageQueue() {
        auto res = rt_mq_detach(&_mq);
        INF_ASSERT(res == RT_EOK);
    }

    rt_err_t StaticMessageQueue::init(const char *name, rt_size_t msg_size, rt_size_t pool_size, rt_uint8_t flag) {
        return rt_mq_init(&_mq, name, _msg_pool, msg_size, pool_size, flag);
    }

    rt_err_t StaticMessageQueue::send(void *buffer, rt_size_t size) {
        return rt_mq_send(&_mq, buffer, size);
    }

    rt_err_t StaticMessageQueue::send(void *buffer, rt_size_t size, rt_int32_t timeout) {
        return rt_mq_send_wait(&_mq, buffer, size, timeout);
    }

    rt_err_t StaticMessageQueue::urgent(void *buffer, rt_size_t size) {
        return rt_mq_urgent(&_mq, buffer, size);
    }

    rt_err_t StaticMessageQueue::receiveTo(rt_ubase_t *recv_buffer, rt_size_t size, rt_int32_t timeout) {
        return rt_mq_recv(&_mq, recv_buffer, size, timeout);
    }

    DynamicMessageQueue::DynamicMessageQueue(
            const char *name, rt_size_t msg_size, rt_size_t max_msgs, rt_uint8_t flag
    ) {
        _mq_t = rt_mq_create(name, msg_size, max_msgs, flag);
        INF_ASSERT(_mq_t != RT_NULL);
    }

    DynamicMessageQueue::~DynamicMessageQueue() {
        auto res = rt_mq_delete(_mq_t);
        INF_ASSERT(res == RT_EOK);
    }

    rt_err_t DynamicMessageQueue::send(void *buffer, rt_size_t size) {
        return rt_mq_send(_mq_t, buffer, size);
    }

    rt_err_t DynamicMessageQueue::send(void *buffer, rt_size_t size, rt_int32_t timeout) {
        return rt_mq_send_wait(_mq_t, buffer, size, timeout);
    }

    rt_err_t DynamicMessageQueue::urgent(void *buffer, rt_size_t size) {
        return rt_mq_urgent(_mq_t, buffer, size);
    }

    rt_err_t DynamicMessageQueue::receiveTo(rt_ubase_t *recv_buffer, rt_size_t size, rt_int32_t timeout) {
        return rt_mq_recv(_mq_t, recv_buffer, size, timeout);
    }
} // infantry
