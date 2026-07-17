#include "virtual_planner/domain/enums/goal_period.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(GoalPeriod goal_period)
{
    switch (goal_period)
    {
       case GoalPeriod::Weekly:
            return "Weekly";

        case GoalPeriod::Monthly:
            return "Monthly";

        case GoalPeriod::Yearly:
            return "Yearly";
    }
 
    throw std::invalid_argument("Invalid GoalPeriod");
}

GoalPeriod goal_period_from_string(std::string_view value)
{
    if (value == "Weekly") return GoalPeriod::Weekly;
    if (value == "Monthly") return GoalPeriod::Monthly;
    if (value == "Yearly") return GoalPeriod::Yearly;

    throw std::invalid_argument("Invalid GoalPeriod");
}

} // namespace virtual_planner::domain