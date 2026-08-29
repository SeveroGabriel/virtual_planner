#include "virtual_planner/application/goal/get_goal_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/persistence/memory/in_memory_goal_repository.hpp"
#include "virtual_planner/shared/errors.hpp"

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

    application::GetGoalUseCase use_case(repository);

    const auto goal = use_case.execute(1);

    VP_EXPECT(
        goal.id() == 1,
        "should return the requested goal");

    VP_EXPECT(
        goal.description() == "Study C++",
        "should return the requested goal data");

    bool not_found_thrown = false;

    try
    {
        const auto missing_goal = use_case.execute(999);
        static_cast<void>(missing_goal);
    }
    catch (const shared::NotFoundError&)
    {
        not_found_thrown = true;
    }

    VP_EXPECT(
        not_found_thrown,
        "getting a missing goal should throw NotFoundError");

    return 0;
}