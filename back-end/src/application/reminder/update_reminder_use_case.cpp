#include "virtual_planner/application/reminder/update_reminder_use_case.hpp"

#include <stdexcept>

namespace virtual_planner::application {

UpdateReminderUseCase::UpdateReminderUseCase(
    persistence::ReminderRepository& repository)
    : repositorio_(repository)
{
}

void UpdateReminderUseCase::execute(
    const UpdateReminderRequest& request) const
{
    const auto existente = repositorio_.find_by_id(request.id);

    if (!existente.has_value())
    {
        throw std::runtime_error("Lembrete não encontrado.");
    }

    const domain::Reminder atualizado{
        existente->id(),
        request.description,
        request.category,
        request.date,
        request.time_slot,
        request.type,
        request.recurrence};

    repositorio_.save(atualizado);
}

} // namespace virtual_planner::application
