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
 * @LastEditTime: 2024-01-17 01:56:00
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#include "rtthread.h"

#include "infantry_def.h"
#include "frame/Inc/infantry_thread.h"
#include "plugins/logger/Inc/infantry_log.h"

namespace infantry {

    DynamicThread::DynamicThread(
            void (*main_func)(void *), const char *name, rt_uint32_t stack_size, rt_uint8_t priority, rt_uint32_t tick,
            void *parameter
    ) : _mainFunc(main_func) {
        _thread = rt_thread_create(
                name, _mainFunc, parameter, stack_size, priority, tick
        );
    }

    void DynamicThread::run() {
        auto res = rt_thread_startup(_thread);
        INF_ASSERT(res == RT_EOK);
    }

    DynamicThread::~DynamicThread() {
        if (rt_thread_delete(_thread) == RT_EOK) {
            logVerboseWithTag("thread", "delete a dynamic task: %p\n", _thread);
        } else {
            logVerboseWithTag("thread", "delete a dynamic task error: %p\n", _thread);
        }
    }

    ThreadPool::ThreadPool(uint32_t max_thread_num)
            : _max_thread_num{max_thread_num} {
        _thread_vec.reserve(max_thread_num);
    }

    void ThreadPool::destroy() {
        if (is_shutdown) {
            logErrorWithTag("tp", "tp is already destroyed");
            while (true) {
                //TODO: error handler
            };
        }
        is_shutdown = true;
        /* 删除线程池中的线程 */
        for (auto &th: _thread_vec) {
            delete th;
        }
        _thread_vec.clear();
        /* 删除任务队列中的任务 */
        while (!task_queue.empty()) { task_queue.pop(); }
        /* 删除用户互斥锁 */
        logDebugWithTag("tp", "Thread pool destroy success");
    }

    ThreadPool::~ThreadPool() {
        destroy();
    }

    ThreadPool *ThreadPool::init(uint32_t thread_stack_size) {
        rt_mutex_init(&tasks_mutex, "tp_mutex", RT_IPC_FLAG_FIFO);
        rt_sem_init(&tasks_sem, "tp_sem", 0, RT_IPC_FLAG_FIFO);

        /* 创建线程池线程 */
        for (uint32_t i = 0; i<_max_thread_num; i++) {
            auto *thread_t = inew infantry::DynamicThread{thread_job, "tp", thread_stack_size, 20, 5, (void *) (this)};
            _thread_vec.push_back(thread_t);
            logDebugWithTag("tp", "tp create thread(%p) success,number is %d", thread_t, i + 1);
            delay(10);
            thread_t->run();
        }

        logDebugWithTag("tp", "initialize thread pool success!");
        return this;
    }

    static void thread_job(void *param) {
        std::function<void()> task;
        auto *pool = (ThreadPool *) param;
        while (true) {
            /* 线程池已关闭 */
            if (pool->is_shutdown) {
                return;
            } else if (rt_sem_take(&pool->tasks_sem, INF_SYNC_FOREVER) == RT_EOK) {
                /* 获取任务队列锁 */
                if (rt_mutex_take(&pool->tasks_mutex, INF_SYNC_FOREVER) == RT_EOK) {
                    if (!pool->task_queue.empty()) {
                        task = std::move(pool->task_queue.front());
                        pool->task_queue.pop();
                    }
                    rt_mutex_release(&pool->tasks_mutex);
                }
            }
            if (task != nullptr) {
                task();
            } else {
                logErrorWithTag("tp", "WTF!!");
            }
        }
    }
} // namespace infantry

EXPORT_STATIC_THREAD(show_logo, 1024, 1, 5) {
    print(
            "InfantryFrame.\n"
            "Designed by \033[35mCharlesHsu\033[0m with \033[1;31mheart\033[0m.\n"
    );
}
