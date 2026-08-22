#include "virtual_planner/application/reminder/create_reminder_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/shared/errors.hpp"
#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"

#include <chrono>
#include <stdexcept>

using namespace virtual_planner;

int main()
{
    persistence::InMemoryReminderRepository repositorio;
    application::CreateReminderUseCase criar(repositorio);

    const application::CreateReminderRequest requisicao{
        42,
        "Planejamento semanal",
        domain::Category::Work,
        domain::Date{20, 8, 2026},
        domain::TimeSlot{std::chrono::hours{9}, std::chrono::hours{10}},
        domain::ReminderType::Meeting,
        domain::ReminderRecurrence::Weekly};

    const auto id = criar.execute(requisicao);
    const auto armazenado = repositorio.find_by_id(42);

    VP_EXPECT(id == 42, "a criação deve retornar o ID informado explicitamente");
    VP_EXPECT(armazenado.has_value(), "o lembrete criado deve ser armazenado");
    VP_EXPECT(armazenado->id() == 42, "a criação deve preservar o ID informado");
    VP_EXPECT(armazenado->description() == "Planejamento semanal", "a criação deve preservar a descrição");
    VP_EXPECT(armazenado->category() == domain::Category::Work, "a criação deve preservar a categoria");
    VP_EXPECT((armazenado->date() == domain::Date{20, 8, 2026}), "a criação deve preservar a data");
    VP_EXPECT(armazenado->time_slot().start() == std::chrono::hours{9}, "a criação deve preservar o horário inicial");
    VP_EXPECT(armazenado->time_slot().end() == std::chrono::hours{10}, "a criação deve preservar o horário final");
    VP_EXPECT(armazenado->type() == domain::ReminderType::Meeting, "a criação deve preservar o tipo");
    VP_EXPECT(armazenado->recurrence() == domain::ReminderRecurrence::Weekly, "a criação deve preservar a recorrência");

    bool descricao_invalida_rejeitada = false;

    try
    {
        const auto id_nao_utilizado = criar.execute(application::CreateReminderRequest{
            43,
            "   ",
            domain::Category::Study,
            domain::Date{21, 8, 2026},
            domain::TimeSlot{std::chrono::hours{14}, std::chrono::hours{15}},
            domain::ReminderType::Study,
            domain::ReminderRecurrence::Once});
        (void)id_nao_utilizado;
    }
    catch (const shared::DomainError&)
    {
        descricao_invalida_rejeitada = true;
    }

    VP_EXPECT(descricao_invalida_rejeitada, "a criação deve rejeitar uma descrição em branco");
    VP_EXPECT(!repositorio.find_by_id(43).has_value(), "uma criação inválida não deve persistir um lembrete");

    bool id_duplicado_rejeitado = false;

    try
    {
        const auto id_nao_utilizado = criar.execute(application::CreateReminderRequest{
            42,
            "Outro lembrete",
            domain::Category::Study,
            domain::Date{22, 8, 2026},
            domain::TimeSlot{std::chrono::hours{16}, std::chrono::hours{17}},
            domain::ReminderType::Study,
            domain::ReminderRecurrence::Once});
        (void)id_nao_utilizado;
    }
    catch (const std::runtime_error&)
    {
        id_duplicado_rejeitado = true;
    }

    const auto original = repositorio.find_by_id(42);

    VP_EXPECT(id_duplicado_rejeitado, "a criação deve rejeitar um ID já utilizado");
    VP_EXPECT(original.has_value(), "o lembrete original deve permanecer armazenado após a tentativa de duplicação");
    VP_EXPECT(original->description() == "Planejamento semanal", "a tentativa de duplicação não deve sobrescrever a descrição original");
    VP_EXPECT(original->category() == domain::Category::Work, "a tentativa de duplicação não deve sobrescrever a categoria original");
    VP_EXPECT((original->date() == domain::Date{20, 8, 2026}), "a tentativa de duplicação não deve sobrescrever a data original");
    VP_EXPECT(original->time_slot().start() == std::chrono::hours{9}, "a tentativa de duplicação não deve sobrescrever o horário inicial original");
    VP_EXPECT(original->time_slot().end() == std::chrono::hours{10}, "a tentativa de duplicação não deve sobrescrever o horário final original");
    VP_EXPECT(original->type() == domain::ReminderType::Meeting, "a tentativa de duplicação não deve sobrescrever o tipo original");
    VP_EXPECT(original->recurrence() == domain::ReminderRecurrence::Weekly, "a tentativa de duplicação não deve sobrescrever a recorrência original");

    return 0;
}
