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
