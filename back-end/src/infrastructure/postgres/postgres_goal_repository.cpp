#include "virtual_planner/infrastructure/postgres/postgres_goal_repository.hpp"

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)

#include <pqxx/pqxx>

namespace virtual_planner::infrastructure::postgres {

PostgresGoalRepository::PostgresGoalRepository(
    PostgresDatabase& database)
    : database_(database)
{
}

std::uint64_t PostgresGoalRepository::save(
    const domain::Goal& goal)
    
{
    pqxx::work transaction(database_.connection());

    auto result = transaction.exec(
    R"(
    INSERT INTO goals
    (
        description,
        category,
        status,
        period
    )
    VALUES ($1,$2,$3,$4)
    RETURNING id
    )",
    pqxx::params{
        transaction,
        goal.description(),
        to_string(goal.category()),
        to_string(goal.status()),
        to_string(goal.period())});

    const auto id = result.one_row()["id"].as<std::uint64_t>();

    transaction.commit();

    return id;

}

void PostgresGoalRepository::update(
    const domain::Goal& goal)
{
    pqxx::work transaction(database_.connection());

    transaction.exec(
    R"(
    UPDATE goals
    SET
        description=$1,
        category=$2,
        status=$3,
        period=$4,
        updated_at=CURRENT_TIMESTAMP
    WHERE id=$5
    )",
    pqxx::params{
        transaction,
        goal.description(),
        to_string(goal.category()),
        to_string(goal.status()),
        to_string(goal.period()),
        goal.id()}).no_rows();

    transaction.commit();
}

std::optional<domain::Goal>
PostgresGoalRepository::find_by_id(std::uint64_t id)
{
    pqxx::read_transaction transaction(database_.connection());

    auto result = transaction.exec(
        R"(
            SELECT
                id,
                description,
                category,
                status,
                period
            FROM goals
            WHERE id = $1
        )",
        pqxx::params{transaction, id}
    );

    if (result.empty())
    {
        return std::nullopt;
    }

    const auto& row = result.front();

    return domain::Goal(
        row["id"].as<std::uint64_t>(),
        row["description"].as<std::string>(),
        domain::category_from_string(
            row["category"].as<std::string>()),
        domain::goal_status_from_string(
            row["status"].as<std::string>()),
        domain::goal_period_from_string(
            row["period"].as<std::string>())
    );
}

std::vector<domain::Goal>
PostgresGoalRepository::find_all()
{
    pqxx::read_transaction transaction(database_.connection());

    auto result = transaction.exec(
        R"(
            SELECT
                id,
                description,
                category,
                status,
                period
            FROM goals
            ORDER BY id
        )"
    );

    std::vector<domain::Goal> goals;

    goals.reserve(result.size());

    for (const auto& row : result)
    {
        goals.emplace_back(
            row["id"].as<std::uint64_t>(),
            row["description"].as<std::string>(),
            domain::category_from_string(
                row["category"].as<std::string>()),
            domain::goal_status_from_string(
                row["status"].as<std::string>()),
            domain::goal_period_from_string(
                row["period"].as<std::string>())
        );
    }

    return goals;
}

void PostgresGoalRepository::remove(
    std::uint64_t id)
{
    pqxx::work transaction(database_.connection());

    transaction.exec(
        "DELETE FROM goals WHERE id=$1",
        pqxx::params{transaction, id}).no_rows();

    transaction.commit();
}

} // namespace virtual_planner::infrastructure::postgres

#endif