#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

#include "virtual_planner/persistence/goal_repository.hpp"

namespace virtual_planner::tests {

class FakeGoalRepository final : public persistence::GoalRepository
{
public:
    std::uint64_t save(const domain::Goal& goal) override
{
    const auto id = next_id_++;

    goals_.emplace_back(
        id,
        goal.description(),
        goal.category(),
        goal.status(),
        goal.period());

    return id;
}

    void update(const domain::Goal& goal) override
    {
        for (auto& current : goals_)
        {
            if (current.id() == goal.id())
            {
                current = goal;
                return;
            }
        }
    }

    std::optional<domain::Goal> find_by_id(std::uint64_t id) override
    {
        for (const auto& goal : goals_)
        {
            if (goal.id() == id)
            {
                return goal;
            }
        }

        return std::nullopt;
    }

    std::vector<domain::Goal> find_all() override
    {
        return goals_;
    }

    void remove(std::uint64_t id) override
    {
        goals_.erase(
            std::remove_if(
                goals_.begin(),
                goals_.end(),
                [id](const domain::Goal& goal)
                {
                    return goal.id() == id;
                }),
            goals_.end());
    }

private:
    private:
    std::vector<domain::Goal> goals_;
    std::uint64_t next_id_ = 1;
};
    
} // namespace virtual_planner::tests
