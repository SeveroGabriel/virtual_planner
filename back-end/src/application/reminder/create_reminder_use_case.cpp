#include "virtual_planner/application/reminder/create_reminder_use_case.hpp"

#include "virtual_planner/domain/entities/reminder.hpp"

#include <stdexcept>

namespace virtual_planner::application {

CreateReminderUseCase::CreateReminderUseCase(
    persistence::ReminderRepository& repository)
    : repositorio_(repository)
{
}

std::uint64_t CreateReminderUseCase::execute(
    const CreateReminderRequest& request) const
{
    if (repositorio_.find_by_id(request.id).has_value())
    {
        throw std::runtime_error("Lembrete já existe.");
    }

    const domain::Reminder lembrete{
        request.id,
        request.description,
        request.category,
        request.date,
        request.time_slot,
        request.type,
        request.recurrence};

    repositorio_.save(lembrete);
    return lembrete.id();
}

} // namespace virtual_planner::application
