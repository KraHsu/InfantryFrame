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

#include "plugins/graph_task/Inc/infantry_graph.h"

namespace infantry {
    static void graph_task(rt_event_t event, uint32_t flag, Task *task) {
        uint32_t e;
        if (rt_event_recv(event, flag, RT_EVENT_FLAG_AND, RT_WAITING_FOREVER, &e) == RT_EOK) {
            task->_task();
            rt_event_send(event, 1 << task->_task_id);
        } else {
            logDebugWithTag("graph", "receive flag(%d) error!", flag);
        }
    }

    StaticGraph::StaticGraph(uint8_t max_thread_num)
            : _thread_pool{max_thread_num}, _event{}, _tasks{nullptr}, _tasks_top{0} {}

    StaticGraph *StaticGraph::schedule(uint32_t stack_size = 1024) {
        _thread_pool.init(stack_size);
        rt_event_init(&_event, "graph", RT_IPC_FLAG_PRIO);

        uint32_t front = 0, back = 0;
        for (uint32_t id = 0; id < _tasks_top; id ++) {
            if (_tasks[id]->_degree == 0) {
                _schedule_index_queue[back ++] = id;
            }
        }
        while (back != front) {
            auto v = _tasks[_schedule_index_queue[front ++]];
            uint32_t flag = GraphTaskBeginId;
            for (uint32_t dep = 0; dep < v->_dependents_top; dep ++) {
                uint32_t dep_id = v->_dependents[dep];
                flag |= (1 << _tasks[dep_id]->_task_id);
            }

            _schedule_flag_queue[front - 1] = flag;

            for (uint32_t suc = 0; suc < v->_successors_top; suc ++) {
                uint32_t suc_id = v->_successors[suc];
                if (! -- (_tasks[suc_id]->_degree)) {
                    _schedule_index_queue[back ++] = suc_id;
                }
            }
        }
        if (back != _tasks_top) {
            logErrorWithTag("grapher", "sort error!!!!!!!! back%d, top%d", back, _tasks_top);
        }
        return this;
    }

    StaticGraph *StaticGraph::loadTask() {
        for (uint32_t i = 0; i < _tasks_top; i ++) {
            auto v = _tasks[_schedule_index_queue[i]];
            auto flag = _schedule_flag_queue[i];
            _thread_pool.add_task(graph_task, &_event, flag, v);
        }
        return this;
    }

   void StaticGraph::execute(int32_t execute_mode) {
        uint32_t e;
        bool is_infinite_loop = (execute_mode == INF_TASK_EXECUTE_LOOP);
        int32_t iteration_count = is_infinite_loop ? - 1 : execute_mode;

        while (is_infinite_loop || iteration_count -- > 0) {
            loadTask();
            rt_event_send(&_event, GraphTaskBeginId);
            if (rt_event_recv(&_event, GraphTaskEndId, INF_EVENT_AND | INF_EVENT_CLEAR, INF_SYNC_FOREVER, &e) !=
                RT_EOK) {
                logErrorWithTag("graph", "error occur when execute the task!");
            }
        }
    }

}
