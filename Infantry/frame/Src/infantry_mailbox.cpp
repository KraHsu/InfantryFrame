/**
 * @file Infantry/frame/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 11/24/2023
 */
#include "infantry_mailbox.h"
#include "infantry_log.h"

namespace infantry {
    DynamicMailBox::DynamicMailBox(const char *name, rt_size_t size, rt_uint8_t flag) {
        _mailbox_t = rt_mb_create(name, size, flag);
        INF_ASSERT(_mailbox_t != RT_NULL);
    }

    DynamicMailBox::~DynamicMailBox() {
        auto res = rt_mb_delete(_mailbox_t);
        INF_ASSERT(res == RT_EOK);
    }

    rt_err_t DynamicMailBox::send(rt_uint32_t value) {
        return rt_mb_send(_mailbox_t, value);
    }

    rt_err_t DynamicMailBox::send(rt_uint32_t value, rt_int32_t timeout) {
        return rt_mb_send_wait(_mailbox_t, value, timeout);
    }

    rt_err_t DynamicMailBox::receiveTo(rt_ubase_t *recv_buffer, rt_int32_t timeout) {
        return rt_mb_recv(_mailbox_t, recv_buffer, timeout);
    }

    StaticMailBox::StaticMailBox(void *msg_pool)
            : _mailbox() {}

    StaticMailBox::~StaticMailBox() {
        auto res = rt_mb_detach(&_mailbox);
        INF_ASSERT(res == RT_EOK);
    }

    rt_err_t StaticMailBox::init(const char *name, rt_size_t size, rt_uint8_t flag) {
        return rt_mb_init(&_mailbox, name, _msg_pool, size, flag);
    }


    rt_err_t StaticMailBox::send(rt_uint32_t value) {
        return rt_mb_send(&_mailbox, value);
    }

    rt_err_t StaticMailBox::send(rt_uint32_t value, rt_int32_t timeout) {
        return rt_mb_send_wait(&_mailbox, value, timeout);
    }

    rt_err_t StaticMailBox::receiveTo(rt_ubase_t *recv_buffer, rt_int32_t timeout) {
        return rt_mb_recv(&_mailbox, (rt_ubase_t *) recv_buffer, timeout);
    }
}