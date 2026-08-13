#include "virtual_planner/application/goal/list_goals_use_case.hpp"

namespace virtual_planner::application {

ListGoalsUseCase::ListGoalsUseCase(
    persistence::GoalRepository& repository)
    : repository_(repository)
{
}

std::vector<domain::Goal>
ListGoalsUseCase::execute() const
{
    return repository_.find_all();
}

} // namespace virtual_planner::application