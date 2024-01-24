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
 * @LastEditTime: 2024-01-18 00:55:18
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include "main.h"
#include "rtthread.h"
#include "preprocessor.hpp"

#define DECLARE_EVENTS(index, name, event) BOOST_PP_IF(BOOST_PP_SUB(index, 1), BOOST_PP_COMMA, BOOST_PP_EMPTY)() \
event = 1u << BOOST_PP_SUB(index, 1)

#define DECLARE_STATIC_EVENTS(NAME, ...) \
BOOST_PP_IF( \
    BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)), 32), \
    "#error : The maximum number of events is 32!", \
    BOOST_PP_EMPTY() \
) \
enum class NAME##Events : uint32_t { \
        BOOST_PP_SEQ_FOR_EACH(DECLARE_EVENTS, NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
}; \
extern infantry::StaticEvent NAME

#define DEFINE_STATIC_EVENTS(NAME) infantry::StaticEvent NAME

#define GET_EVENTS_INNER(index, name, event) BOOST_PP_IF(BOOST_PP_SUB(index, 1), |, BOOST_PP_EMPTY()) \
static_cast<uint32_t>(name##Events::event)
#define GET_EVENTS(NAME, ...) \
BOOST_PP_SEQ_FOR_EACH(GET_EVENTS_INNER, NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) // NOLINT

namespace infantry {
    class StaticEvent {
    private:
        rt_event _event;
    public:
        StaticEvent( ) = default;

        ~StaticEvent( );

        /**
         * @brief 初始化事件集
         * @param name <const char *> 事件集的名称
         * @param flag [INF_SYNC_FIFO|INF_SYNC_PRIO] 信号量属性
         * @see INF_SYNC_FIFO 先进先出, INF_SYNC_PRIO 按优先级
         * @return
         */
        rt_err_t init(const char *name, rt_uint8_t flag);

        /**
         * @brief 发送事件集中的一个或多个事件
         * @param set <uint32_t> 发送的一个或多个事件的标志值
         * @return
         */
        rt_err_t send(rt_uint32_t set);

        /**
         * @brief 等待事件
         * @param set <uint32_t> 等待的事件集
         * @param option [INF_EVENT_OR|INF_EVENT_AND]
         * @param timeout [int32_t|INF_SYNC_FOREVER] 超时时间
         * @param received <uint32_t *> [out] 接收到的事件
         * @see INF_EVENT_OR 逻辑或，INF_EVENT_AND 逻辑与
         * @return
         */
        rt_err_t receive(rt_uint32_t set, rt_uint8_t option, rt_int32_t timeout, rt_uint32_t *received = nullptr);
    };

    class DynamicEvent {
    private:
        rt_event_t _event_t;
    public:
        DynamicEvent(const char *name, rt_uint8_t flag);

        ~DynamicEvent( );

        /**
         * @brief 发送事件集中的一个或多个事件
         * @param set <uint32_t> 发送的一个或多个事件的标志值
         * @return
         */
        rt_err_t send(rt_uint32_t set);

        /**
         * @brief 等待事件
         * @param set <uint32_t> 等待的事件集
         * @param option [INF_EVENT_OR|INF_EVENT_AND]
         * @param timeout [int32_t|INF_SYNC_FOREVER] 超时时间
         * @param received <uint32_t *> [out] 接收到的事件
         * @see INF_EVENT_OR 逻辑或，INF_EVENT_AND 逻辑与
         * @return
         */
        rt_err_t receive(rt_uint32_t set, rt_uint8_t option, rt_int32_t timeout, rt_uint32_t *recved);
    };
}
