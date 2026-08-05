#include <cassert>

#include "virtual_planner/application/goal/create_goal_use_case.hpp"

#include "../../persistence/fake_goal_repository.hpp"

using namespace virtual_planner;

int main()
{
    tests::FakeGoalRepository repository;

    application::CreateGoalUseCase create(repository);

    application::CreateGoalRequest request{
        "Finish Paradigms project",
        domain::Category::Study,
        domain::GoalPeriod::Weekly};

    create.execute(request);

    auto goals = repository.find_all();

    assert(goals.size() == 1);

    assert(goals.front().description() ==
           "Finish Paradigms project");

    return 0;
}
