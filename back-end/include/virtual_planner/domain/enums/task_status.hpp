#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {
    
enum class TaskStatus
{
    Pending,
    Executed,
    PartiallyExecuted,
    Cancelled,
    Postponed
};

std::string to_string(TaskStatus value);

TaskStatus task_status_from_string(std::string_view value);

} // namespace virtual_planner::domain