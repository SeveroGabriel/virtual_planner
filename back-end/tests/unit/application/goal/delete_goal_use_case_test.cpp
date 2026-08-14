#include "virtual_planner/application/goal/delete_goal_use_case.hpp"

#include "support/expect.hpp"
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

    VP_EXPECT(repository.find_all().empty(), "repository should be empty after deleting the only goal");

    return 0;
}
