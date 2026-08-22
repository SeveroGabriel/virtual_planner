#include "virtual_planner/application/reminder/delete_reminder_use_case.hpp"

#include <stdexcept>

namespace virtual_planner::application {

DeleteReminderUseCase::DeleteReminderUseCase(
    persistence::ReminderRepository& repository)
    : repositorio_(repository)
{
}

void DeleteReminderUseCase::execute(std::uint64_t id) const
{
    if (!repositorio_.find_by_id(id).has_value())
    {
        throw std::runtime_error("Lembrete não encontrado.");
    }

    repositorio_.remove(id);
}

} // namespace virtual_planner::application
