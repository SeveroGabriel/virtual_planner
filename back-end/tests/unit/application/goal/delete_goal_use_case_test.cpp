#include "virtual_planner/application/goal/delete_goal_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/persistence/memory/in_memory_goal_repository.hpp"

using namespace virtual_planner;

int main()
{
    persistence::InMemoryGoalRepository repository;

    repository.save(
        domain::Goal(
            1,
            "Study C++",
            domain::Category::Study,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Weekly,
            domain::Date(10, 8, 2026)));

    application::DeleteGoalUseCase remove(repository);

    remove.execute(1);

    VP_EXPECT(repository.find_all().empty(), "repository should be empty after deleting the only goal");

    return 0;
}
