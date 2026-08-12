#include <cassert>

#include "virtual_planner/application/goal/delete_goal_use_case.hpp"

#include "../../persistence/fake_goal_repository.hpp"

using namespace virtual_planner;

int main()
{
    tests::FakeGoalRepository repository;

    repository.save(
        domain::Goal(
            1,
            "Study C++",
            domain::Category::Study,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Weekly));

    application::DeleteGoalUseCase remove(repository);

    remove.execute(1);

    assert(repository.find_all().empty());

    return 0;
}
