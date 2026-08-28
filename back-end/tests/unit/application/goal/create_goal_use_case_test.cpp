#include "virtual_planner/application/goal/create_goal_use_case.hpp"
#include "support/expect.hpp"
#include "virtual_planner/persistence/memory/in_memory_goal_repository.hpp"

using namespace virtual_planner;

int main()
{
    persistence::InMemoryGoalRepository repository;

    application::CreateGoalUseCase create(repository);

    application::CreateGoalRequest request{
        "Finish Paradigms project",
        domain::Category::Study,
        domain::GoalPeriod::Weekly,
        domain::Date(10, 8, 2026)};

    const auto id = create.execute(request);

    VP_EXPECT(id != 0, "created goal id should be non-zero");

    auto goals = repository.find_all();

    VP_EXPECT(goals.size() == 1, "repository should contain exactly one goal after creation");

    VP_EXPECT(goals.front().description() ==
                  "Finish Paradigms project",
              "created goal should keep the requested description");

    VP_EXPECT(goals.front().reference_date() == domain::Date(10, 8, 2026),
              "created goal should keep the requested reference date");

    return 0;
}
