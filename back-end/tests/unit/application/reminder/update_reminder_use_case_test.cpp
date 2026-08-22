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
    persistence::InMemoryReminderRepository repositorio;
    repositorio.save(domain::Reminder{
        7,
        "Original",
        domain::Category::Study,
        domain::Date{20, 8, 2026},
        domain::TimeSlot{std::chrono::hours{9}, std::chrono::hours{10}},
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Once});

    application::UpdateReminderUseCase atualizar(repositorio);
    atualizar.execute(application::UpdateReminderRequest{
        7,
        "Atualizado",
        domain::Category::Work,
        domain::Date{27, 8, 2026},
        domain::TimeSlot{std::chrono::hours{14}, std::chrono::hours{16}},
        domain::ReminderType::Meeting,
        domain::ReminderRecurrence::Weekly});

    const auto atualizado = repositorio.find_by_id(7);

    VP_EXPECT(atualizado.has_value(), "o lembrete atualizado deve continuar existindo");
    VP_EXPECT(atualizado->id() == 7, "a atualização deve preservar o ID");
    VP_EXPECT(atualizado->description() == "Atualizado", "a atualização deve substituir a descrição");
    VP_EXPECT(atualizado->category() == domain::Category::Work, "a atualização deve substituir a categoria");
    VP_EXPECT((atualizado->date() == domain::Date{27, 8, 2026}), "a atualização deve substituir a data");
    VP_EXPECT(atualizado->time_slot().start() == std::chrono::hours{14}, "a atualização deve substituir o horário inicial");
    VP_EXPECT(atualizado->time_slot().end() == std::chrono::hours{16}, "a atualização deve substituir o horário final");
    VP_EXPECT(atualizado->type() == domain::ReminderType::Meeting, "a atualização deve substituir o tipo");
    VP_EXPECT(atualizado->recurrence() == domain::ReminderRecurrence::Weekly, "a atualização deve substituir a recorrência");
    VP_EXPECT(repositorio.find_all().size() == 1, "a atualização não deve adicionar outra entidade");

    bool inexistente_rejeitado = false;

    try
    {
        atualizar.execute(application::UpdateReminderRequest{
            999,
            "Inexistente",
            domain::Category::Work,
            domain::Date{1, 9, 2026},
            domain::TimeSlot{std::chrono::hours{8}, std::chrono::hours{9}},
            domain::ReminderType::Meeting,
            domain::ReminderRecurrence::Once});
    }
    catch (const std::runtime_error& error)
    {
        inexistente_rejeitado = std::string{error.what()} == "Lembrete não encontrado.";
    }

    VP_EXPECT(inexistente_rejeitado, "a atualização deve informar um ID inexistente");

    bool atualizacao_invalida_rejeitada = false;

    try
    {
        atualizar.execute(application::UpdateReminderRequest{
            7,
            " ",
            domain::Category::Health,
            domain::Date{1, 1, 2027},
            domain::TimeSlot{std::chrono::hours{18}, std::chrono::hours{19}},
            domain::ReminderType::Exercise,
            domain::ReminderRecurrence::Daily});
    }
    catch (const shared::DomainError&)
    {
        atualizacao_invalida_rejeitada = true;
    }

    const auto apos_invalida = repositorio.find_by_id(7);
    VP_EXPECT(atualizacao_invalida_rejeitada, "a atualização deve rejeitar uma descrição inválida");
    VP_EXPECT(apos_invalida->description() == "Atualizado", "a atualização rejeitada deve preservar a descrição");
    VP_EXPECT(apos_invalida->category() == domain::Category::Work, "a atualização rejeitada deve preservar a categoria");
    VP_EXPECT((apos_invalida->date() == domain::Date{27, 8, 2026}), "a atualização rejeitada deve preservar a data");
    VP_EXPECT(apos_invalida->time_slot().start() == std::chrono::hours{14},
    "a atualização rejeitada deve preservar o horário inicial");

    VP_EXPECT(apos_invalida->time_slot().end() == std::chrono::hours{16},
    "a atualização rejeitada deve preservar o horário final");
    VP_EXPECT(apos_invalida->type() == domain::ReminderType::Meeting, "a atualização rejeitada deve preservar o tipo");
    VP_EXPECT(apos_invalida->recurrence() == domain::ReminderRecurrence::Weekly, "a atualização rejeitada deve preservar a recorrência");
    VP_EXPECT(repositorio.find_all().size() == 1,
    "a atualização rejeitada não deve adicionar outra entidade");

    return 0;
}
