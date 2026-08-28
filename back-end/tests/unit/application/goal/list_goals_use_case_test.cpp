#include "virtual_planner/application/goal/list_goals_use_case.hpp"
#include <stdexcept>
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
            domain::Date(5, 8, 2026)));

    repository.save(
        domain::Goal(
            2,
            "Finish Planner",
            domain::Category::Work,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Monthly,
            domain::Date(20, 8, 2026)));

    application::ListGoalsUseCase use_case(repository);

    auto goals = use_case.execute(
        domain::Date(1, 8, 2026),
        domain::Date(10, 8, 2026));

    VP_EXPECT(
        goals.size() == 1,
        "should return only goals inside the requested period");

    VP_EXPECT(
        goals.front().description() == "Study C++",
        "should return the goal inside the requested period");

    goals = use_case.execute(
        domain::Date(15, 8, 2026),
        domain::Date(25, 8, 2026));

    VP_EXPECT(
        goals.size() == 1,
        "should return goals inside the second requested period");

    VP_EXPECT(
        goals.front().description() == "Finish Planner",
        "should return the goal inside the second requested period");

    goals = use_case.execute(
        domain::Date(5, 8, 2026),
        domain::Date(5, 8, 2026));

    VP_EXPECT(
        goals.size() == 1,
        "should include goals on the exact date boundaries");

    VP_EXPECT(
        goals.front().description() == "Study C++",
        "should include a goal whose reference date equals the boundaries");

    goals = use_case.execute(
        domain::Date(1, 9, 2026),
        domain::Date(10, 9, 2026));

    VP_EXPECT(
        goals.empty(),
        "should return an empty list when no goals are inside the period");

    // Arrange
    bool rejected_inverted_range = false;

    // Act
    try
    {
        const auto unexpected_goals = use_case.execute(
            domain::Date(11, 9, 2026),
            domain::Date(10, 9, 2026));
        static_cast<void>(unexpected_goals);
    }
    catch (const std::invalid_argument&)
    {
        rejected_inverted_range = true;
    }

    // Assert
    VP_EXPECT(
        rejected_inverted_range,
        "should reject an inverted goal date range");

    return 0;
}
