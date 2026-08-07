#include <cassert>

#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_database.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_goal_repository.hpp"


using namespace virtual_planner;

int main()
{
    infrastructure::postgres::PostgresConfig config{
        "localhost",
        5432,
        "planner",
        "postgres",
        "postgres"
    };

    infrastructure::postgres::PostgresDatabase database(config);

    database.initialize();
    database.connect();

    infrastructure::postgres::PostgresGoalRepository repository(database);

    domain::Goal goal(
        0,
        "Finish C++ Planner",
        domain::Category::Study,
        domain::GoalStatus::InProgress,
        domain::GoalPeriod::Weekly);

    repository.save(goal);

    auto goals = repository.find_all();

    assert(!goals.empty());

    database.shutdown();

    return 0;
}