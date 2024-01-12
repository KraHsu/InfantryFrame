/**
 * @file Infantry/plugins/graph_task/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 11/28/2023
 */
#include "infantry_task.h"

namespace infantry {
    Task::Task(uint32_t task_id, std::function<void()> &&task)
            : _task_id{task_id}, _task{std::move(task)}, _dependents_top{0}, _successors_top{0}, _successors{0},
              _dependents{0}, _degree{0} {}

    void Task::addDependency(Task *task) {
        _dependents[_dependents_top++] = task->_task_id;
        task->_successors[task->_successors_top++] = _task_id;
        _degree++;
    }

    Task &Task::operator--() {
        return *this;
    }

    Task &Task::operator--(int) {
        return *this;
    }

    Task &Task::operator>(Task &other) {
        other.addDependency(this);
        return other;
    }

    Task &Task::operator<(Task &other) {
        addDependency(&other);
        return other;
    }
} // infantry
