#include "virtual_planner/application/reminder/list_reminders_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"

#include <chrono>
#include <stdexcept>

using namespace virtual_planner;

namespace {

// O id so existe na assinatura porque Reminder o exige no construtor; o
// repositorio o descarta e gera o seu (issue #90). Os testes que precisam do
// id usam o valor devolvido por save.
domain::Reminder make_reminder(
    std::uint64_t id,
    domain::Date date,
    domain::ReminderType type,
    domain::ReminderRecurrence recurrence,
    std::chrono::hours start = std::chrono::hours{9})
{
    return domain::Reminder{
        id,
        "Lembrete",
        domain::Category::Study,
        date,
        domain::TimeSlot{start, start + std::chrono::hours{1}},
        type,
        recurrence};
}

application::ListRemindersRequest window(
    domain::Date start,
    domain::Date end,
    std::optional<domain::ReminderType> type = std::nullopt,
    std::optional<domain::ReminderRecurrence> recurrence = std::nullopt)
{
    return application::ListRemindersRequest{start, end, type, recurrence};
}

} // namespace

int main()
{
    {
        persistence::InMemoryReminderRepository repository;
        application::ListRemindersUseCase list_reminders(repository);

        VP_EXPECT(
            list_reminders.execute(window(domain::Date{1, 8, 2026}, domain::Date{31, 8, 2026})).empty(),
            "um repositório vazio deve produzir uma lista de ocorrências vazia");
    }

    {
        persistence::InMemoryReminderRepository repository;
        repository.save(make_reminder(1, domain::Date{10, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once));
        repository.save(make_reminder(2, domain::Date{10, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Weekly));
        repository.save(make_reminder(3, domain::Date{10, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Once));
        application::ListRemindersUseCase list_reminders(repository);

        const auto by_type = list_reminders.execute(window(
            domain::Date{10, 8, 2026}, domain::Date{10, 8, 2026}, domain::ReminderType::Meeting));
        VP_EXPECT(by_type.size() == 2, "o filtro por tipo deve manter os lembretes correspondentes");

        const auto by_recurrence = list_reminders.execute(window(
            domain::Date{10, 8, 2026}, domain::Date{17, 8, 2026}, std::nullopt, domain::ReminderRecurrence::Once));
        VP_EXPECT(by_recurrence.size() == 2, "o filtro por recorrência deve manter os lembretes correspondentes");

        const auto combined = list_reminders.execute(window(
            domain::Date{10, 8, 2026}, domain::Date{17, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Weekly));
        VP_EXPECT(combined.size() == 2, "os filtros por tipo e recorrência devem usar semântica AND");
        VP_EXPECT(combined[0].reminder.id() == 2 && combined[1].reminder.id() == 2, "os filtros combinados devem excluir lembretes não correspondentes");
    }

    {
        persistence::InMemoryReminderRepository repository;
        repository.save(make_reminder(10, domain::Date{15, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once));
        application::ListRemindersUseCase list_reminders(repository);

        const auto within_window = list_reminders.execute(window(domain::Date{15, 8, 2026}, domain::Date{15, 8, 2026}));
        const auto outside_window = list_reminders.execute(window(domain::Date{16, 8, 2026}, domain::Date{20, 8, 2026}));

        VP_EXPECT(within_window.size() == 1, "a recorrência única deve ocorrer exatamente uma vez na data-base");
        VP_EXPECT(outside_window.empty(), "a recorrência única não deve se repetir após a data-base");
    }

    {
        persistence::InMemoryReminderRepository repository;
        repository.save(make_reminder(20, domain::Date{10, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Daily));
        application::ListRemindersUseCase list_reminders(repository);

        const auto occurrences = list_reminders.execute(window(domain::Date{8, 8, 2026}, domain::Date{12, 8, 2026}));

        VP_EXPECT(occurrences.size() == 3, "a recorrência diária deve incluir a data-base e cada dia seguinte dentro da janela");
        VP_EXPECT((occurrences.front().occurrence_date == domain::Date{10, 8, 2026}), "a recorrência não deve gerar ocorrências anteriores à data-base");
        VP_EXPECT((occurrences.back().occurrence_date == domain::Date{12, 8, 2026}), "o fim da janela deve ser inclusivo");
    }

    {
        persistence::InMemoryReminderRepository repository;
        repository.save(make_reminder(30, domain::Date{3, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Weekly));
        application::ListRemindersUseCase list_reminders(repository);

        const auto occurrences = list_reminders.execute(window(domain::Date{9, 8, 2026}, domain::Date{24, 8, 2026}));

        VP_EXPECT(occurrences.size() == 3, "o lembrete semanal iniciado antes da janela deve gerar ocorrências dentro dela");
        VP_EXPECT((occurrences[0].occurrence_date == domain::Date{10, 8, 2026}), "a ocorrência semanal deve estar sete dias após a data-base");
        VP_EXPECT((occurrences[1].occurrence_date == domain::Date{17, 8, 2026}), "as ocorrências semanais devem permanecer separadas por sete dias");
        VP_EXPECT((occurrences[2].occurrence_date == domain::Date{24, 8, 2026}), "a ocorrência semanal no limite final inclusivo deve aparecer");
    }

    {
        persistence::InMemoryReminderRepository repository;
        repository.save(make_reminder(40, domain::Date{31, 1, 2026}, domain::ReminderType::Shopping, domain::ReminderRecurrence::Monthly));
        application::ListRemindersUseCase list_reminders(repository);

        const auto occurrences = list_reminders.execute(window(domain::Date{31, 1, 2026}, domain::Date{30, 4, 2026}));

        VP_EXPECT(occurrences.size() == 4, "a recorrência mensal deve produzir uma ocorrência por mês");
        VP_EXPECT((occurrences[0].occurrence_date == domain::Date{31, 1, 2026}), "a recorrência mensal deve incluir sua data-base");
        VP_EXPECT((occurrences[1].occurrence_date == domain::Date{28, 2, 2026}), "a recorrência mensal deve ajustar para o último dia válido");
        VP_EXPECT((occurrences[2].occurrence_date == domain::Date{31, 3, 2026}), "a recorrência mensal deve retornar ao dia-âncora original");
        VP_EXPECT((occurrences[3].occurrence_date == domain::Date{30, 4, 2026}), "a recorrência mensal deve ajustar cada mês curto de forma independente");
    }

    {
        persistence::InMemoryReminderRepository repository;
        repository.save(make_reminder(50, domain::Date{31, 1, 2024}, domain::ReminderType::Assignment, domain::ReminderRecurrence::Monthly));
        application::ListRemindersUseCase list_reminders(repository);

        const auto occurrences = list_reminders.execute(window(domain::Date{29, 2, 2024}, domain::Date{29, 2, 2024}));
        VP_EXPECT(occurrences.size() == 1, "uma janela de um dia deve incluir uma ocorrência correspondente");
        VP_EXPECT((occurrences.front().occurrence_date == domain::Date{29, 2, 2024}), "a recorrência mensal deve respeitar o dia bissexto ao ajustar a data");
    }

    {
        persistence::InMemoryReminderRepository repository;

        // O id agora vem do repositorio (issue #90), entao o teste guarda o
        // que save devolveu em vez de escolher os valores. A ordem de
        // insercao e proposital: o das 8h entra no meio, para que passar no
        // teste dependa da ordenacao e nao da ordem de chegada.
        const auto first_id_at_nine = repository.save(make_reminder(0, domain::Date{20, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once, std::chrono::hours{9}));
        const auto id_at_eight = repository.save(make_reminder(0, domain::Date{20, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once, std::chrono::hours{8}));
        const auto second_id_at_nine = repository.save(make_reminder(0, domain::Date{20, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once, std::chrono::hours{9}));
        application::ListRemindersUseCase list_reminders(repository);

        VP_EXPECT(first_id_at_nine < second_id_at_nine, "os ids gerados devem crescer na ordem de insercao");

        const auto size_before = repository.find_all().size();
        const auto occurrences = list_reminders.execute(window(domain::Date{20, 8, 2026}, domain::Date{20, 8, 2026}));

        VP_EXPECT(occurrences.size() == 3, "uma janela inclusiva de um dia deve incluir todas as ocorrências correspondentes");
        VP_EXPECT(occurrences[0].reminder.id() == id_at_eight, "os resultados devem ser ordenados pelo horário inicial após a data");
        VP_EXPECT(occurrences[1].reminder.id() == first_id_at_nine, "os resultados devem usar o ID como desempate final");
        VP_EXPECT(occurrences[2].reminder.id() == second_id_at_nine, "os resultados devem usar o ID como desempate final");
        VP_EXPECT(repository.find_all().size() == size_before, "a expansão não deve adicionar entidades ao repositório");
        VP_EXPECT((repository.find_by_id(id_at_eight)->date() == domain::Date{20, 8, 2026}), "a expansão deve preservar as datas-base persistidas");
    }

    {
        persistence::InMemoryReminderRepository repository;
        application::ListRemindersUseCase list_reminders(repository);
        bool inverted_window_rejected = false;

        try
        {
            const auto unused = list_reminders.execute(
                window(domain::Date{2, 8, 2026}, domain::Date{1, 8, 2026}));
            (void)unused;
        }
        catch (const std::invalid_argument&)
        {
            inverted_window_rejected = true;
        }

        VP_EXPECT(inverted_window_rejected, "uma janela de datas invertida deve ser rejeitada");
    }

    return 0;
}
