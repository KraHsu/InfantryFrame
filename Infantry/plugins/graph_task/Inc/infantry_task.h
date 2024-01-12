/**
 * @file Infantry/plugins/graph_task/Src
 * @author CharlesHsu
 * @date 11/28/2023
 */
#pragma once

#include "infantry_thread.h"

#include <vector>
#include <functional>

namespace infantry {
    class Task {
    public:
        std::function<void()> _task; // 任务的执行逻辑
        uint32_t _dependents[32]; // 直接依赖的任务
        uint32_t _dependents_top;
        uint32_t _successors[32]; // 直接被依赖的任务
        uint32_t _successors_top;
        uint32_t _degree; // 入度计数
        uint32_t _task_id;
    public:
        Task(uint32_t task_id, std::function<void()> &&task);

        void addDependency(Task *task);

        Task &operator--();

        Task &operator--(int);

        Task &operator>(Task &other);

        Task &operator<(Task &other);

    };
} // infantry
