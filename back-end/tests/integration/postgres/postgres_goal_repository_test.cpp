#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_database.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_goal_repository.hpp"

#include "support/expect.hpp"

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
        VP_EXPECT(id != 0, "save() must return a non-zero id");

        auto saved_goal = repository.find_by_id(id);

        VP_EXPECT(
            saved_goal.has_value(),
            "find_by_id() must return the saved goal");
        VP_EXPECT(saved_goal->id() == id, "saved goal id must match");
        VP_EXPECT(
            saved_goal->description() == "Finish C++ Planner",
            "saved goal description must match");
        VP_EXPECT(
            saved_goal->category() == domain::Category::Study,
            "saved goal category must match");
        VP_EXPECT(
            saved_goal->status() == domain::GoalStatus::InProgress,
            "saved goal status must match");
        VP_EXPECT(
            saved_goal->period() == domain::GoalPeriod::Weekly,
            "saved goal period must match");

        // Assert find_all()
        const auto goals = repository.find_all();

        const auto found = std::any_of(
            goals.begin(),
            goals.end(),
            [id](const domain::Goal& current)
            {
                return current.id() == id;
            });

        VP_EXPECT(found, "find_all() must include the saved goal");

        // Act: update() — this is the path that used to fail because the
        // 001 schema had no updated_at column while the repository set it
        // unconditionally (see migrations/002_add_goals_timestamps_and_checks.sql).
        domain::Goal updated_goal(
            id,
            "Finish C++ Planner (revised)",
            domain::Category::PersonalProjects,
            domain::GoalStatus::Completed,
            domain::GoalPeriod::Monthly);

        repository.update(updated_goal);

        // Assert
        auto reloaded_goal = repository.find_by_id(id);

        VP_EXPECT(
            reloaded_goal.has_value(),
            "find_by_id() must return the updated goal");
        VP_EXPECT(
            reloaded_goal->id() == id,
            "updated goal id must be unchanged");
        VP_EXPECT(
            reloaded_goal->description() ==
                "Finish C++ Planner (revised)",
            "update() must persist the new description");
        VP_EXPECT(
            reloaded_goal->category() ==
                domain::Category::PersonalProjects,
            "update() must persist the new category");
        VP_EXPECT(
            reloaded_goal->status() == domain::GoalStatus::Completed,
            "update() must persist the new status");
        VP_EXPECT(
            reloaded_goal->period() == domain::GoalPeriod::Monthly,
            "update() must persist the new period");

        // Cleanup
        repository.remove(id);

        const auto removed_goal = repository.find_by_id(id);

        VP_EXPECT(
            !removed_goal.has_value(),
            "remove() must delete the goal");

        database.shutdown();

        VP_EXPECT(
            !database.is_connected(),
            "database must be disconnected after shutdown");

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
