#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {

enum class GoalStatus
{
    InProgress,
    Completed,
    PartiallyCompleted,
    Failed
};

std::string to_string(GoalStatus value);

GoalStatus goal_status_from_string(std::string_view value);

} // namespace virtual_planner::domain