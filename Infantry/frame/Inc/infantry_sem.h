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
 * @LastEditTime: 2024-01-18 00:21:55
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include "rtthread.h"

namespace infantry {
    class StaticSem {
    private:
        rt_semaphore _semaphore{};
    public:
        StaticSem() = default;

        ~StaticSem();

        /**
         * @brief 初始化信号量
         * @param name <const char *> 信号量名称
         * @param value <uint32_t> 信号量初始值
         * @param flag [INF_SYNC_FIFO|INF_SYNC_PRIO] 信号量属性
         * @see INF_SYNC_FIFO 先进先出, INF_SYNC_PRIO 按优先级
         * @return
         */
        rt_err_t init(const char *name, rt_uint32_t value, rt_uint8_t flag);

        /**
         * @brief 等待信号量
         * @return
         */
        rt_err_t take();

        /**
         * @brief 等待信号量, 超时返回
         * @param time [int32_t|INF_SYNC_FOREVER] 超时时间
         * @see INF_SYNC_FOREVER 永久等待
         * @return
         */
        rt_err_t take(rt_int32_t time);

        /**
         * @brief 获取信号量值
         * @return
         */
        const rt_uint16_t &getValue() const;

        /**
         * @brief 释放信号量
         * @return
         */
        rt_err_t release();
    };

    class DynamicSem {
    private:
        rt_sem_t _semphore_t;
    public:
        /**
         * @brief 构造信号量
         * @param name <const char *> 信号量名称
         * @param value <uint32_t> 信号量初始值
         * @param flag [INF_SYNC_FIFO|INF_SYNC_PRIO] 信号量属性
         * @see INF_SYNC_FIFO 先进先出, INF_SYNC_PRIO 按优先级
         * @return
         */
        DynamicSem(const char *name, rt_uint32_t value, rt_uint8_t flag);

        ~DynamicSem();

        /**
         * @brief 等待信号量
         * @return
         */
        rt_err_t take();

        /**
         * @brief 等待信号量, 超时返回
         * @param time [int32_t|INF_SYNC_FOREVER] 超时时间
         * @see INF_SYNC_FOREVER 永久等待
         * @return
         */
        rt_err_t take(rt_int32_t time);

        /**
         * @brief 获取信号量值
         * @return
         */
        const rt_uint16_t &getValue() const;

        /**
         * @brief 释放信号量
         * @return
         */
        rt_err_t release();
    };
}
