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
