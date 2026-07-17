#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "virtual_planner/domain/entities/reminder.hpp"

namespace virtual_planner::persistence {

class ReminderRepository
{
public:
    virtual ~ReminderRepository() = default;

    virtual void save(const domain::Reminder& reminder) = 0;

    virtual std::optional<domain::Reminder> find_by_id(
        std::uint64_t id) = 0;

    virtual std::vector<domain::Reminder> find_all() = 0;

    virtual void remove(std::uint64_t id) = 0;
};

} // namespace virtual_planner::persistence