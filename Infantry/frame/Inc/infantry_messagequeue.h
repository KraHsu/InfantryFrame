/**
 * @file Infantry/frame/Src
 * @author CharlesHsu
 * @date 11/28/2023
 */
#pragma once

#include "rtthread.h"

namespace infantry {

    class StaticMessageQueue {
    private:
        rt_messagequeue _mq;
        void *_msg_pool;
    public:
        explicit StaticMessageQueue(void *msg_pool);

        ~StaticMessageQueue();

        rt_err_t init(const char *name, rt_size_t msg_size, rt_size_t pool_size, rt_uint8_t flag);

        rt_err_t send(void *buffer, rt_size_t size);

        rt_err_t send(void *buffer, rt_size_t size, rt_int32_t timeout);

        rt_err_t urgent(void *buffer, rt_size_t size);

        rt_err_t receiveTo(rt_ubase_t *recv_buffer, rt_size_t size, rt_int32_t timeout);
    };

    class DynamicMessageQueue {
    private:
        rt_mq_t _mq_t;
    public:
        explicit DynamicMessageQueue(const char *name, rt_size_t msg_size, rt_size_t max_msgs, rt_uint8_t flag);

        ~DynamicMessageQueue();

        rt_err_t send(void *buffer, rt_size_t size);

        rt_err_t send(void *buffer, rt_size_t size, rt_int32_t timeout);

        rt_err_t urgent(void *buffer, rt_size_t size);

        rt_err_t receiveTo(rt_ubase_t *recv_buffer, rt_size_t size, rt_int32_t timeout);
    };

} // infantry
