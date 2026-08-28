#pragma once
#include <cstdint>

#include "virtual_planner/infrastructure/postgres/postgres_database.hpp"
#include "virtual_planner/persistence/goal_repository.hpp"

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)

namespace virtual_planner::infrastructure::postgres
{

class PostgresGoalRepository final
    : public persistence::GoalRepository
{
public:
    explicit PostgresGoalRepository(
        PostgresDatabase& database);

    std::uint64_t save(const domain::Goal& goal) override;

    void update(const domain::Goal& goal) override;

    std::optional<domain::Goal> find_by_id(
        std::uint64_t id) override;

    std::vector<domain::Goal> find_all() override;

    std::vector<domain::Goal> find_by_date_range(
        const domain::Date& start_date,
        const domain::Date& end_date) override;

    void remove(std::uint64_t id) override;

private:
    PostgresDatabase& database_;
};

} // namespace virtual_planner::infrastructure::postgres

#endif