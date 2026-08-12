#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_database.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_goal_repository.hpp"

using namespace virtual_planner;

namespace
{

bool has_postgres_environment()
{
    return std::getenv("POSTGRES_DB") != nullptr &&
           std::getenv("POSTGRES_USER") != nullptr &&
           std::getenv("POSTGRES_PASSWORD") != nullptr;
}

}

int main()
{
    using infrastructure::postgres::PostgresConfig;
    using infrastructure::postgres::PostgresDatabase;
    using infrastructure::postgres::PostgresGoalRepository;

    if (!has_postgres_environment())
    {
        std::cout
            << "Skipping PostgreSQL goal repository test: "
            << "POSTGRES_DB, POSTGRES_USER and POSTGRES_PASSWORD "
            << "are required.\n";

        return 0;
    }

    try
    {
        // Arrange
        PostgresDatabase database(
            PostgresConfig::from_environment());

        database.initialize();
        database.connect();

        PostgresGoalRepository repository(database);

        domain::Goal goal(
            0,
            "Finish C++ Planner",
            domain::Category::Study,
            domain::GoalStatus::InProgress,
            domain::GoalPeriod::Weekly);

        // Act
        const auto id = repository.save(goal);

        // Assert
        assert(id != 0);

        auto saved_goal = repository.find_by_id(id);

        assert(saved_goal.has_value());
        assert(saved_goal->id() == id);
        assert(saved_goal->description() == "Finish C++ Planner");
        assert(saved_goal->category() == domain::Category::Study);
        assert(
            saved_goal->status() ==
            domain::GoalStatus::InProgress);
        assert(
            saved_goal->period() ==
            domain::GoalPeriod::Weekly);

        // Assert find_all()
        const auto goals = repository.find_all();

        const auto found = std::any_of(
            goals.begin(),
            goals.end(),
            [id](const domain::Goal& current)
            {
                return current.id() == id;
            });

        assert(found);

        // Cleanup
        repository.remove(id);

        const auto removed_goal = repository.find_by_id(id);

        assert(!removed_goal.has_value());

        database.shutdown();

        assert(!database.is_connected());

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}