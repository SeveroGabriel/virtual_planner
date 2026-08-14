#include "virtual_planner/application/goal/list_goals_use_case.hpp"
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

    repository.save(
        domain::Goal(
            2,
            "Finish Planner",
            domain::Category::Work,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Monthly));

    application::ListGoalsUseCase use_case(repository);

    auto goals = use_case.execute();

    VP_EXPECT(goals.size() == 2, "listing goals should return every saved goal");

    return 0;
}
