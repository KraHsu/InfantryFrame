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

    rt_err_t StaticEvent::receive(rt_uint32_t set, rt_uint8_t option, rt_int32_t timeout, rt_uint32_t *received) {
        if (received == nullptr) {
            uint32_t tmp;
            return rt_event_recv(&_event, set, option, timeout, &tmp);
        }
        return rt_event_recv(&_event, set, option, timeout, received);
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
