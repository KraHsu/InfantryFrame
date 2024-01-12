/**
 * @file Infantry/frame/Src
 * @author CharlesHsu
 * @date 11/24/2023
 */
#pragma once

#include "rtthread.h"

namespace infantry {
    class StaticMailBox {
    private:
        rt_mailbox _mailbox;
        void *_msg_pool;
    public:
        explicit StaticMailBox(void *msg_pool);

        ~StaticMailBox();

        rt_err_t init(const char *name, rt_size_t size, rt_uint8_t flag);

        rt_err_t send(rt_uint32_t value);

        rt_err_t send(rt_uint32_t value, rt_int32_t timeout);

        rt_err_t receiveTo(rt_ubase_t *recv_buffer, rt_int32_t timeout);
    };

    class DynamicMailBox {
    private:
        rt_mailbox_t _mailbox_t;
    public:
        DynamicMailBox(const char *name, rt_size_t size, rt_uint8_t flag);

        ~DynamicMailBox();

        rt_err_t send(rt_uint32_t value);

        rt_err_t send(rt_uint32_t value, rt_int32_t timeout);

        rt_err_t receiveTo(rt_ubase_t *recv_buffer, rt_int32_t timeout);
    };
}
