#pragma once

#include <cstdint>
#include <string>

#include "virtual_planner/domain/enums/category.hpp"
#include "virtual_planner/domain/enums/goal_period.hpp"
#include "virtual_planner/persistence/goal_repository.hpp"

namespace virtual_planner::application {

struct CreateGoalRequest
{
    std::string description;
    domain::Category category;
    domain::GoalPeriod period;
};

class CreateGoalUseCase
{
public:
    explicit CreateGoalUseCase(
        persistence::GoalRepository& repository);

    [[nodiscard]] std::uint64_t execute(
        const CreateGoalRequest& request);

private:
    persistence::GoalRepository& repository_;
};

} // namespace virtual_planner::application