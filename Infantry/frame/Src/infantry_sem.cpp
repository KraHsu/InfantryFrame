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

    const rt_uint16_t &DynamicSem::getValue() const {
        return _semphore_t->value;
    }
}
