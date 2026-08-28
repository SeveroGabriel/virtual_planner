#include "virtual_planner/application/reminder/update_reminder_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/shared/errors.hpp"
#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"

#include <chrono>
#include <stdexcept>
#include <string>

using namespace virtual_planner;

int main()
{
    persistence::InMemoryReminderRepository repository;

    // O id vem do repositorio (issue #90); a entidade entra com 0 e o valor
    // gerado e o que vale daqui em diante.
    const auto id = repository.save(domain::Reminder{
        0,
        "Original",
        domain::Category::Study,
        domain::Date{20, 8, 2026},
        domain::TimeSlot{std::chrono::hours{9}, std::chrono::hours{10}},
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Once});

    application::UpdateReminderUseCase update_reminder(repository);
    update_reminder.execute(application::UpdateReminderRequest{
        id,
        "Atualizado",
        domain::Category::Work,
        domain::Date{27, 8, 2026},
        domain::TimeSlot{std::chrono::hours{14}, std::chrono::hours{16}},
        domain::ReminderType::Meeting,
        domain::ReminderRecurrence::Weekly});

    const auto updated = repository.find_by_id(id);

    VP_EXPECT(updated.has_value(), "o lembrete atualizado deve continuar existindo");
    VP_EXPECT(updated->id() == id, "a atualização deve preservar o ID");
    VP_EXPECT(updated->description() == "Atualizado", "a atualização deve substituir a descrição");
    VP_EXPECT(updated->category() == domain::Category::Work, "a atualização deve substituir a categoria");
    VP_EXPECT((updated->date() == domain::Date{27, 8, 2026}), "a atualização deve substituir a data");
    VP_EXPECT(updated->time_slot().start() == std::chrono::hours{14}, "a atualização deve substituir o horário inicial");
    VP_EXPECT(updated->time_slot().end() == std::chrono::hours{16}, "a atualização deve substituir o horário final");
    VP_EXPECT(updated->type() == domain::ReminderType::Meeting, "a atualização deve substituir o tipo");
    VP_EXPECT(updated->recurrence() == domain::ReminderRecurrence::Weekly, "a atualização deve substituir a recorrência");
    VP_EXPECT(repository.find_all().size() == 1, "a atualização não deve adicionar outra entidade");

    bool unknown_id_rejected = false;

    try
    {
        update_reminder.execute(application::UpdateReminderRequest{
            id + 999,
            "Inexistente",
            domain::Category::Work,
            domain::Date{1, 9, 2026},
            domain::TimeSlot{std::chrono::hours{8}, std::chrono::hours{9}},
            domain::ReminderType::Meeting,
            domain::ReminderRecurrence::Once});
    }
    catch (const std::runtime_error& error)
    {
        unknown_id_rejected = std::string{error.what()} == "Lembrete não encontrado.";
    }

    VP_EXPECT(unknown_id_rejected, "a atualização deve informar um ID inexistente");

    bool invalid_update_rejected = false;

    try
    {
        update_reminder.execute(application::UpdateReminderRequest{
            id,
            " ",
            domain::Category::Health,
            domain::Date{1, 1, 2027},
            domain::TimeSlot{std::chrono::hours{18}, std::chrono::hours{19}},
            domain::ReminderType::Exercise,
            domain::ReminderRecurrence::Daily});
    }
    catch (const shared::DomainError&)
    {
        invalid_update_rejected = true;
    }

    const auto after_invalid = repository.find_by_id(id);
    VP_EXPECT(invalid_update_rejected, "a atualização deve rejeitar uma descrição inválida");
    VP_EXPECT(after_invalid->description() == "Atualizado", "a atualização rejeitada deve preservar a descrição");
    VP_EXPECT(after_invalid->category() == domain::Category::Work, "a atualização rejeitada deve preservar a categoria");
    VP_EXPECT((after_invalid->date() == domain::Date{27, 8, 2026}), "a atualização rejeitada deve preservar a data");
    VP_EXPECT(after_invalid->time_slot().start() == std::chrono::hours{14},
    "a atualização rejeitada deve preservar o horário inicial");

    VP_EXPECT(after_invalid->time_slot().end() == std::chrono::hours{16},
    "a atualização rejeitada deve preservar o horário final");
    VP_EXPECT(after_invalid->type() == domain::ReminderType::Meeting, "a atualização rejeitada deve preservar o tipo");
    VP_EXPECT(after_invalid->recurrence() == domain::ReminderRecurrence::Weekly, "a atualização rejeitada deve preservar a recorrência");
    VP_EXPECT(repository.find_all().size() == 1,
    "a atualização rejeitada não deve adicionar outra entidade");

    return 0;
}
