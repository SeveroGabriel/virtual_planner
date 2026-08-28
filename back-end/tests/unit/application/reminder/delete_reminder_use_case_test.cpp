#include "virtual_planner/application/reminder/delete_reminder_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"

#include <chrono>
#include <stdexcept>
#include <string>

using namespace virtual_planner;

int main()
{
    persistence::InMemoryReminderRepository repository;

    // O id vem do repositorio (issue #90).
    const auto id = repository.save(domain::Reminder{
        0,
        "Excluir este lembrete",
        domain::Category::PersonalProjects,
        domain::Date{20, 8, 2026},
        domain::TimeSlot{std::chrono::hours{9}, std::chrono::hours{10}},
        domain::ReminderType::PhoneCall,
        domain::ReminderRecurrence::Once});

    application::DeleteReminderUseCase delete_reminder(repository);
    delete_reminder.execute(id);

    VP_EXPECT(!repository.find_by_id(id).has_value(), "a exclusão deve remover o lembrete existente");

    bool unknown_id_rejected = false;

    try
    {
        delete_reminder.execute(id);
    }
    catch (const std::runtime_error& error)
    {
        unknown_id_rejected = std::string{error.what()} == "Lembrete não encontrado.";
    }

    VP_EXPECT(unknown_id_rejected, "a exclusão deve informar um ID inexistente");

    return 0;
}
