#include <cassert>

#include "virtual_planner/application/goal/update_goal_use_case.hpp"

#include "../../persistence/fake_goal_repository.hpp"

using namespace virtual_planner;

int main()
{
    tests::FakeGoalRepository repository;

    repository.save(
        domain::Goal(
            1,
            "Old",
            domain::Category::Study,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Weekly));

    application::UpdateGoalUseCase update(repository);

    application::UpdateGoalRequest request{
        1,
        "New Description",
        domain::Category::Work,
        domain::GoalPeriod::Monthly};

    update.execute(request);

    auto goal = repository.find_by_id(1);

    assert(goal.has_value());

    assert(goal->description() ==
           "New Description");

    return 0;
}
