#include "virtual_planner/application/goal/delete_goal_use_case.hpp"

#include <stdexcept>

namespace virtual_planner::application {

DeleteGoalUseCase::DeleteGoalUseCase(
    persistence::GoalRepository& repository)
    : repository_(repository)
{
}

void DeleteGoalUseCase::execute(std::uint64_t id)
{
    auto goal = repository_.find_by_id(id);

    if (!goal.has_value())
    {
        throw std::runtime_error(
            "Goal not found.");
    }

    repository_.remove(id);
}

}