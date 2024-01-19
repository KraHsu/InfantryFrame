/*
 *                                |~~~~~~~|
 *                                |       |
 *                                |       |
 *                                |       |
 *                                |       |
 *                                |       |
 *     |~.\\\_\~~~~~~~~~~~~~~xx~~~         ~~~~~~~~~~~~~~~~~~~~~/_//;~|
 *     |  \  o \_         ,XXXXX),                         _..-~ o /  |
 *     |    ~~\  ~-.     XXXXX`)))),                 _.--~~   .-~~~   |
 *      ~~~~~~~`\   ~\~~~XXX' _/ ';))     |~~~~~~..-~     _.-~ ~~~~~~~
 *               `\   ~~--`_\~\, ;;;\)__.---.~~~      _.-~
 *                 ~-.       `:;;/;; \          _..-~~
 *                    ~-._      `''        /-~-~
 *                        `\              /  /
 *                          |         ,   | |
 *                           |  '        /  |
 *                            \/;          |
 *                             ;;          |
 *                             `;   .       |
 *                             |~~~-----.....|
 *                            | \             \
 *                           | /\~~--...__    |
 *                           (|  `\       __-\|
 *                           ||    \_   /~    |
 *                           |)     \~-'      |
 *                            |      | \      '
 *                            |      |  \    :
 *                             \     |  |    |
 *                              |    )  (    )
 *                               \  /;  /\  |
 *                               |    |/   |
 *                               |    |   |
 *                                \  .'  ||
 *                                |  |  | |
 *                                (  | |  |
 *                                |   \ \ |
 *                                || o `.)|
 *                                |`\\) |
 *                                |       |
 *                                |       |
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:55:18
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
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
