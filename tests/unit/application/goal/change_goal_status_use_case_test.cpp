#include <cassert>

#include "virtual_planner/application/goal/change_goal_status_use_case.hpp"
#include "../../persistence/fake_goal_repository.hpp"

using namespace virtual_planner;

int main()
{
    tests::FakeGoalRepository repository;

    repository.save(
        domain::Goal(
            1,
            "Finish Planner",
            domain::Category::Study,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Weekly));

    application::ChangeGoalStatusUseCase use_case(repository);

    application::ChangeGoalStatusRequest request{
        1,
        domain::GoalStatus::Completed
    };

    use_case.execute(request);

    auto goal = repository.find_by_id(1);

    assert(goal.has_value());

    assert(goal->status() ==
           domain::GoalStatus::Completed);

    return 0;
}