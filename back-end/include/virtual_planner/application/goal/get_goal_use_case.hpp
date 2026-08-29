#pragma once

#include <cstdint>

#include "virtual_planner/domain/entities/goal.hpp"
#include "virtual_planner/persistence/goal_repository.hpp"

namespace virtual_planner::application {

class GetGoalUseCase
{
public:
    explicit GetGoalUseCase(
        persistence::GoalRepository& repository);

    [[nodiscard]]
    domain::Goal execute(std::uint64_t id) const;

private:
    persistence::GoalRepository& repository_;
};

} // namespace virtual_planner::application
