#include "virtual_planner/domain/enums/goal_status.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(GoalStatus goal_status)
{
    switch (goal_status)
    {
        case GoalStatus::InProgress:
            return "In Progress";

        case GoalStatus::Completed:
            return "Completed";

        case GoalStatus::PartiallyCompleted:
            return "Partially Completed";

        case GoalStatus::Failed:
            return "Failed";
    }
 
    throw std::invalid_argument("Invalid GoalStatus");
}

GoalStatus goal_status_from_string(std::string_view value)
{
    if (value == "In Progress") return GoalStatus::InProgress;
    if (value == "Completed") return GoalStatus::Completed;
    if (value == "Partially Completed") return GoalStatus::PartiallyCompleted;
    if (value == "Failed") return GoalStatus::Failed;

    throw std::invalid_argument("Invalid GoalStatus");
}

}  // namespace virtual_planner::domain