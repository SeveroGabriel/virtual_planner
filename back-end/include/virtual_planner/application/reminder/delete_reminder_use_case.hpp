#pragma once

#include <cstdint>

#include "virtual_planner/persistence/reminder_repository.hpp"

namespace virtual_planner::application {

class DeleteReminderUseCase
{
public:
    explicit DeleteReminderUseCase(
        persistence::ReminderRepository& repository);

    void execute(std::uint64_t id) const;

private:
    persistence::ReminderRepository& repositorio_;
};

} // namespace virtual_planner::application
