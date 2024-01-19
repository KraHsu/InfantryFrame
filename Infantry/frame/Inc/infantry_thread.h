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
 * @LastEditTime: 2024-01-17 01:55:40
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once


#include "infantry_def.h"
#include "frame/Inc/infantry_thread.h"
#include "frame/Inc/infantry_mutex.h"
#include "frame/Inc/infantry_sem.h"
#include "infantry_log.h"
#include "infantry_cplus.h"

#include <functional>
#include <queue>

namespace infantry {
    class Thread {
    protected:
    public:
        virtual ~Thread() = default;

        virtual void run() = 0;
    };

    class DynamicThread : public Thread {
    private:
        rt_thread_t _thread{};

        void (*_mainFunc)(void *);

    public:
        DynamicThread(
                void (*main_func)(void *), const char *name, rt_uint32_t stack_size, rt_uint8_t priority,
                rt_uint32_t tick, void *parameter = RT_NULL
        );

        void run() override;

        ~DynamicThread() override;
    };

    class ThreadPool;
	
    static void thread_job(void *pool_ptr);

    class ThreadPool {
    private:
        uint32_t _max_thread_num{0}; // 最大线程数
        std::vector<infantry::DynamicThread *> _thread_vec;
    public:
        bool is_shutdown{false};
        rt_mutex tasks_mutex{};
        rt_semaphore tasks_sem{};
        std::queue<std::function<void()>> task_queue{};
    public:
        explicit ThreadPool(uint32_t max_thread_num);

        ThreadPool *init(uint32_t thread_stack_size);

        template<typename Callable, typename... Args>
        ThreadPool *add_task(Callable &&task, Args... args) {
            // 获取任务队列互斥锁,向线程池中添加任务
            if (rt_mutex_take(&tasks_mutex, INF_SYNC_FOREVER) == RT_EOK) {
                // 将任务加入到任务队列中
                task_queue.emplace([args..., &task]() { task(args...); });
                // 释放一个任务数
                rt_sem_release(&tasks_sem);
                // 释放任务队列互斥锁
                rt_mutex_release(&tasks_mutex);
                logDebugWithTag("tp", "add a task success.");
            } else {
                logDebugWithTag("tp", "add a task failed.");
            }
            return this;
        };

        void destroy();

        ~ThreadPool();
    };
} // namespace infantry
