#pragma once

#include <cstdint>
#include <string>

#include "virtual_planner/domain/enums/category.hpp"
#include "virtual_planner/domain/enums/goal_period.hpp"
#include "virtual_planner/persistence/goal_repository.hpp"

namespace virtual_planner::application {

struct UpdateGoalRequest
{
    std::uint64_t id;

    std::string description;

    domain::Category category;

    domain::GoalPeriod period;
};

class UpdateGoalUseCase
{
public:
    explicit UpdateGoalUseCase(
        persistence::GoalRepository& repository);

    void execute(const UpdateGoalRequest& request);

private:
    persistence::GoalRepository& repository_;
};

} // namespace virtual_planner::application