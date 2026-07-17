#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {

enum class GoalPeriod
{
    Weekly,
    Monthly,
    Yearly
};

std::string to_string(GoalPeriod value);

GoalPeriod goal_period_from_string(std::string_view value);

} // namespace virtual_planner::domain