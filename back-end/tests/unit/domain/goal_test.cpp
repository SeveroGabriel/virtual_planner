#include "virtual_planner/domain/entities/goal.hpp"
#include "../../support/expect.hpp"

using namespace virtual_planner::domain;

int main()
{
    Goal goal(
        42,
        "Finish C++ Planner",
        Category::Study,
        GoalStatus::InProgress,
        GoalPeriod::Weekly,
        Date(10, 8, 2026)
    );

    // Constructor and getters

    VP_EXPECT(
        goal.id() == 42,
        "goal id should be initialized correctly"
    );

    VP_EXPECT(
        goal.description() == "Finish C++ Planner",
        "goal description should be initialized correctly"
    );

    VP_EXPECT(
        goal.category() == Category::Study,
        "goal category should be initialized correctly"
    );

    VP_EXPECT(
        goal.status() == GoalStatus::InProgress,
        "goal status should be initialized correctly"
    );

    VP_EXPECT(
        goal.period() == GoalPeriod::Weekly,
        "goal period should be initialized correctly"
    );

    VP_EXPECT(
        goal.reference_date() == Date(10, 8, 2026),
        "goal reference date should be initialized correctly"
    );

    // Description

    goal.update_description("Finish PostgreSQL integration");

    VP_EXPECT(
        goal.description() == "Finish PostgreSQL integration",
        "update_description should change the goal description"
    );

    // Category

    goal.change_category(Category::Work);

    VP_EXPECT(
        goal.category() == Category::Work,
        "change_category should change the goal category"
    );

    // Period

    goal.change_period(GoalPeriod::Monthly);

    VP_EXPECT(
        goal.period() == GoalPeriod::Monthly,
        "change_period should change the goal period"
    );

    // Reference date

    goal.change_reference_date(Date(20, 8, 2026));

    VP_EXPECT(
        goal.reference_date() == Date(20, 8, 2026),
        "change_reference_date should change the goal reference date"
    );

    // Status

    goal.mark_as_completed();

    VP_EXPECT(
        goal.status() == GoalStatus::Completed,
        "mark_as_completed should set status to Completed"
    );

    goal.mark_as_partially_completed();

    VP_EXPECT(
        goal.status() == GoalStatus::PartiallyCompleted,
        "mark_as_partially_completed should set status to PartiallyCompleted"
    );

    goal.mark_as_failed();

    VP_EXPECT(
        goal.status() == GoalStatus::Failed,
        "mark_as_failed should set status to Failed"
    );

    goal.mark_as_in_progress();

    VP_EXPECT(
        goal.status() == GoalStatus::InProgress,
        "mark_as_in_progress should set status to InProgress"
    );

    return 0;
}
