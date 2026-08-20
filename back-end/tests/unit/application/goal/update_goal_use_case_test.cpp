#include "virtual_planner/application/goal/update_goal_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/persistence/memory/in_memory_goal_repository.hpp"

using namespace virtual_planner;

int main()
{
    persistence::InMemoryGoalRepository repository;

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

    VP_EXPECT(goal.has_value(), "goal must exist after update");

    VP_EXPECT(goal->description() ==
                  "New Description",
              "updated goal should have the new description");

    return 0;
}
