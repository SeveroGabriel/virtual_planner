#include "virtual_planner/application/goal/update_goal_use_case.hpp"

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
            "Old",
            domain::Category::Study,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Weekly,
            domain::Date(10, 8, 2026)));

    application::UpdateGoalUseCase update(repository);

    application::UpdateGoalRequest request{
        1,
        "New Description",
        domain::Category::Work,
        domain::GoalPeriod::Monthly,
        domain::Date(20, 8, 2026)
    };

    update.execute(request);

    auto goal = repository.find_by_id(1);

    VP_EXPECT(
        goal.has_value(),
        "goal must exist after update");

    VP_EXPECT(
        goal->description() == "New Description",
        "updated goal should have the new description");

    VP_EXPECT(
        goal->category() == domain::Category::Work,
        "updated goal should have the new category");

    VP_EXPECT(
        goal->period() == domain::GoalPeriod::Monthly,
        "updated goal should have the new period");

    VP_EXPECT(
        goal->reference_date() == domain::Date(20, 8, 2026),
        "updated goal should have the new reference date");

    bool not_found_thrown = false;

    try
    {
        application::UpdateGoalRequest missing_request{
            999,
            "Missing",
            domain::Category::Study,
            domain::GoalPeriod::Weekly,
            domain::Date(20, 8, 2026)
        };

        update.execute(missing_request);
    }
    catch (const shared::NotFoundError&)
    {
        not_found_thrown = true;
    }

    VP_EXPECT(
        not_found_thrown,
        "updating a missing goal should throw NotFoundError");

    return 0;
}

