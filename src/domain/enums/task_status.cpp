#include "virtual_planner/domain/enums/task_status.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(TaskStatus task_status)
{
    switch (task_status)
    {
        case TaskStatus::Pending:
            return "Pending";

        case TaskStatus::Executed:
            return "Executed";

        case TaskStatus::PartiallyExecuted:
            return "PartiallyExecuted";

        case TaskStatus::Cancelled:
            return "Cancelled";

        case TaskStatus::Postponed:
            return "Postponed";
    }

    throw std::invalid_argument("Invalid TaskStatus.");
}

TaskStatus task_status_from_string(std::string_view value)
{
    if (value == "Pending") return TaskStatus::Pending;
    if (value == "Executed") return TaskStatus::Executed;
    if (value == "PartiallyExecuted") return TaskStatus::PartiallyExecuted;
    if (value == "Cancelled") return TaskStatus::Cancelled;
    if (value == "Postponed") return TaskStatus::Postponed;

    throw std::invalid_argument("Invalid TaskStatus.");
}

} // namespace virtual_planner::domain