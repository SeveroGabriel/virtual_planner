#include "virtual_planner/application/reminder/create_reminder_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/shared/errors.hpp"
#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"

#include <chrono>

using namespace virtual_planner;

int main()
{
    persistence::InMemoryReminderRepository repository;
    application::CreateReminderUseCase create_reminder(repository);

    const application::CreateReminderRequest request{
        "Planejamento semanal",
        domain::Category::Work,
        domain::Date{20, 8, 2026},
        domain::TimeSlot{std::chrono::hours{9}, std::chrono::hours{10}},
        domain::ReminderType::Meeting,
        domain::ReminderRecurrence::Weekly};

    const auto id = create_reminder.execute(request);
    const auto stored = repository.find_by_id(id);

    VP_EXPECT(id != 0, "a criação deve retornar o ID gerado pelo repositório");
    VP_EXPECT(stored.has_value(), "o lembrete criado deve ser armazenado");
    VP_EXPECT(stored->id() == id, "o lembrete armazenado deve ter o ID retornado");
    VP_EXPECT(stored->description() == "Planejamento semanal", "a criação deve preservar a descrição");
    VP_EXPECT(stored->category() == domain::Category::Work, "a criação deve preservar a categoria");
    VP_EXPECT((stored->date() == domain::Date{20, 8, 2026}), "a criação deve preservar a data");
    VP_EXPECT(stored->time_slot().start() == std::chrono::hours{9}, "a criação deve preservar o horário inicial");
    VP_EXPECT(stored->time_slot().end() == std::chrono::hours{10}, "a criação deve preservar o horário final");
    VP_EXPECT(stored->type() == domain::ReminderType::Meeting, "a criação deve preservar o tipo");
    VP_EXPECT(stored->recurrence() == domain::ReminderRecurrence::Weekly, "a criação deve preservar a recorrência");

    // --- Regressão da issue #90 ---------------------------------------------
    //
    // Antes, o chamador escolhia o ID e save fazia upsert: criar dois
    // lembretes com o mesmo ID sobrescrevia o primeiro em silêncio. A guarda
    // do #87 protegia só este ponto de chamada, não o contrato. Agora o ID é
    // do repositório e não existe mais como um create apagar outro.
    const auto second_id = create_reminder.execute(application::CreateReminderRequest{
        "Outro lembrete",
        domain::Category::Study,
        domain::Date{22, 8, 2026},
        domain::TimeSlot{std::chrono::hours{16}, std::chrono::hours{17}},
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Once});

    VP_EXPECT(second_id != id, "duas criações devem receber IDs distintos");
    VP_EXPECT(repository.find_all().size() == 2, "duas criações devem resultar em dois lembretes");

    const auto first = repository.find_by_id(id);

    VP_EXPECT(first.has_value(), "o primeiro lembrete deve continuar armazenado");
    VP_EXPECT(first->description() == "Planejamento semanal", "uma criação nova não deve sobrescrever a descrição da anterior");
    VP_EXPECT(first->category() == domain::Category::Work, "uma criação nova não deve sobrescrever a categoria da anterior");
    VP_EXPECT((first->date() == domain::Date{20, 8, 2026}), "uma criação nova não deve sobrescrever a data da anterior");
    VP_EXPECT(first->type() == domain::ReminderType::Meeting, "uma criação nova não deve sobrescrever o tipo da anterior");

    // --- Descrição inválida --------------------------------------------------
    bool blank_description_rejected = false;

    try
    {
        const auto unused_id = create_reminder.execute(application::CreateReminderRequest{
            "   ",
            domain::Category::Study,
            domain::Date{21, 8, 2026},
            domain::TimeSlot{std::chrono::hours{14}, std::chrono::hours{15}},
            domain::ReminderType::Study,
            domain::ReminderRecurrence::Once});
        (void)unused_id;
    }
    catch (const shared::DomainError&)
    {
        blank_description_rejected = true;
    }

    VP_EXPECT(blank_description_rejected, "a criação deve rejeitar uma descrição em branco");
    VP_EXPECT(repository.find_all().size() == 2, "uma criação inválida não deve persistir um lembrete");

    return 0;
}
