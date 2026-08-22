#pragma once

#include <cstdint>
#include <string>

#include "virtual_planner/domain/enums/category.hpp"
#include "virtual_planner/domain/enums/reminder_recurrence.hpp"
#include "virtual_planner/domain/enums/reminder_type.hpp"
#include "virtual_planner/domain/value_objects/date.hpp"
#include "virtual_planner/domain/value_objects/time_slot.hpp"
#include "virtual_planner/persistence/reminder_repository.hpp"

namespace virtual_planner::application {

struct CreateReminderRequest
{
    std::uint64_t id;
    std::string description;
    domain::Category category;
    domain::Date date;
    domain::TimeSlot time_slot;
    domain::ReminderType type;
    domain::ReminderRecurrence recurrence;
};

class CreateReminderUseCase
{
public:
    explicit CreateReminderUseCase(
        persistence::ReminderRepository& repository);

    [[nodiscard]] std::uint64_t execute(
        const CreateReminderRequest& request) const;

private:
    persistence::ReminderRepository& repositorio_;
};

} // namespace virtual_planner::application
