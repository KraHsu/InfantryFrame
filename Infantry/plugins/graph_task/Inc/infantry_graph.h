/*
 * _______________#########_______________________ 
 * ______________############_____________________ 
 * ______________#############____________________ 
 * _____________##__###########___________________ 
 * ____________###__######_#####__________________ 
 * ____________###_#######___####_________________ 
 * ___________###__##########_####________________ 
 * __________####__###########_####_______________ 
 * ________#####___###########__#####_____________ 
 * _______######___###_########___#####___________ 
 * _______#####___###___########___######_________ 
 * ______######___###__###########___######_______ 
 * _____######___####_##############__######______ 
 * ____#######__#####################_#######_____ 
 * ____#######__##############################____ 
 * ___#######__######_#################_#######___ 
 * ___#######__######_######_#########___######___ 
 * ___#######____##__######___######_____######___ 
 * ___#######________######____#####_____#####____ 
 * ____######________#####_____#####_____####_____ 
 * _____#####________####______#####_____###______ 
 * ______#####______;###________###______#________ 
 * ________##_______####________####______________ 
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:56:37
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include "infantry_task.h"
#include "frame/Inc/infantry_messagequeue.h"
#include "frame/Inc/infantry_sem.h"
#include "frame/Inc/infantry_thread.h"
#include "frame/Inc/infantry_event.h"

#define GraphTaskBeginId (1u << 31)
#define GraphTaskEndId ((1 << _tasks_top) - 1 | GraphTaskBeginId)
namespace infantry {

    class StaticGraph {
    private:
        Task *_tasks[32]; // 所有任务的列表
        uint32_t _tasks_top;
        uint32_t _schedule_index_queue[32]{}; // 拓扑排序后的索引列表
        uint32_t _schedule_flag_queue[32]{}; // 拓扑排序后的 flag 列表
        ThreadPool _thread_pool;
        struct rt_event _event;
    public:
        explicit StaticGraph(uint8_t max_thread_num);

        template<typename Callable, typename... Args>
        Task &createTask(Callable task, Args... args) {
            INF_ASSERT((1 << _tasks_top) < GraphTaskBeginId);
            _tasks[_tasks_top] = inew Task(_tasks_top, [&]() { task(args...); });
            return *_tasks[_tasks_top ++];
        }

        StaticGraph *schedule(uint32_t stack_size);

        StaticGraph *loadTask();

        void execute(int32_t execute_mode);
    };
}

