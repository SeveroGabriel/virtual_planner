#include "virtual_planner/application/goal/change_goal_status_use_case.hpp"

#include <stdexcept>

namespace virtual_planner::application {

ChangeGoalStatusUseCase::ChangeGoalStatusUseCase(
    persistence::GoalRepository& repository)
    : repository_(repository)
{
}

void ChangeGoalStatusUseCase::execute(
    const ChangeGoalStatusRequest& request)
{
    auto goal = repository_.find_by_id(request.id);

    if (!goal.has_value())
    {
        throw std::runtime_error(
            "Goal not found.");
    }

    switch (request.status)
    {
        case domain::GoalStatus::InProgress:
            goal->mark_as_in_progress();
            break;

        case domain::GoalStatus::Completed:
            goal->mark_as_completed();
            break;

        case domain::GoalStatus::PartiallyCompleted:
            goal->mark_as_partially_completed();
            break;

        case domain::GoalStatus::Failed:
            goal->mark_as_failed();
            break;
    }

    repository_.update(*goal);
}

} // namespace virtual_planner::application