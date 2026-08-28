#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_database.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_reminder_repository.hpp"

#include "support/expect.hpp"

using namespace virtual_planner;

namespace {

bool has_postgres_environment()
{
    return std::getenv("POSTGRES_DB") != nullptr &&
           std::getenv("POSTGRES_USER") != nullptr &&
           std::getenv("POSTGRES_PASSWORD") != nullptr;
}

// A limpeza usa os ids que o proprio banco gerou nesta execucao. Como cada
// execucao recebe ids novos (issue #90), nao ha mais risco de uma rodada
// anterior deixar lixo com id conhecido para trás.
void remove_saved_reminders(
    infrastructure::postgres::PostgresReminderRepository& repository,
    const std::vector<std::uint64_t>& ids)
{
    for (const auto id : ids)
    {
        repository.remove(id);
    }
}

void expect_reminder(
    const domain::Reminder& reminder,
    std::uint64_t id,
    const std::string& description,
    domain::Category category,
    const domain::Date& date,
    std::chrono::minutes start,
    std::chrono::minutes end,
    domain::ReminderType type,
    domain::ReminderRecurrence recurrence)
{
    VP_EXPECT(
        reminder.id() == id,
        "o id do lembrete deve ser preservado após a persistência");
    VP_EXPECT(
        reminder.description() == description,
        "a descrição do lembrete deve ser preservada após a persistência");
    VP_EXPECT(
        reminder.category() == category,
        "a categoria do lembrete deve ser preservada após a persistência");
    VP_EXPECT(
        reminder.date() == date,
        "a data do lembrete deve ser preservada após a persistência");
    VP_EXPECT(
        reminder.time_slot().start() == start,
        "o horário inicial do lembrete deve ser preservado em minutos");
    VP_EXPECT(
        reminder.time_slot().end() == end,
        "o horário final do lembrete deve ser preservado em minutos");
    VP_EXPECT(
        reminder.type() == type,
        "o tipo do lembrete deve ser preservado após a persistência");
    VP_EXPECT(
        reminder.recurrence() == recurrence,
        "a recorrência do lembrete deve ser preservada após a persistência");
}

} // namespace

int main()
{
    using infrastructure::postgres::PostgresConfig;
    using infrastructure::postgres::PostgresDatabase;
    using infrastructure::postgres::PostgresReminderRepository;

    if (!has_postgres_environment())
    {
        std::cout
            << "Teste do repositório PostgreSQL de lembretes ignorado: "
            << "POSTGRES_DB, POSTGRES_USER e POSTGRES_PASSWORD "
            << "são obrigatórias.\n";

        return 0;
    }

    try
    {
        // Preparação
        PostgresDatabase database(PostgresConfig::from_environment());
        database.initialize();
        database.connect();

        PostgresReminderRepository repository(database);

        const std::array<domain::Reminder, 6> reminders{
            domain::Reminder{
                0,
                "Reunião do projeto",
                domain::Category::Work,
                domain::Date{10, 9, 2026},
                domain::TimeSlot{
                    std::chrono::minutes{9 * 60 + 15},
                    std::chrono::minutes{10 * 60 + 45}},
                domain::ReminderType::Meeting,
                domain::ReminderRecurrence::Once},
            domain::Reminder{
                0,
                "Ligar para fornecedor",
                domain::Category::PersonalProjects,
                domain::Date{11, 9, 2026},
                domain::TimeSlot{
                    std::chrono::minutes{11 * 60},
                    std::chrono::minutes{11 * 60 + 30}},
                domain::ReminderType::PhoneCall,
                domain::ReminderRecurrence::Daily},
            domain::Reminder{
                0,
                "Comprar mantimentos",
                domain::Category::Leisure,
                domain::Date{12, 9, 2026},
                domain::TimeSlot{
                    std::chrono::minutes{12 * 60},
                    std::chrono::minutes{13 * 60}},
                domain::ReminderType::Shopping,
                domain::ReminderRecurrence::Weekly},
            domain::Reminder{
                0,
                "Revisar anotações de C++",
                domain::Category::Study,
                domain::Date{13, 9, 2026},
                domain::TimeSlot{
                    std::chrono::minutes{14 * 60},
                    std::chrono::minutes{15 * 60}},
                domain::ReminderType::Study,
                domain::ReminderRecurrence::Monthly},
            domain::Reminder{
                0,
                "Exercício matinal",
                domain::Category::Health,
                domain::Date{14, 9, 2026},
                domain::TimeSlot{
                    std::chrono::minutes{6 * 60},
                    std::chrono::minutes{7 * 60}},
                domain::ReminderType::Exercise,
                domain::ReminderRecurrence::Once},
            domain::Reminder{
                0,
                "Entregar atividade",
                domain::Category::College,
                domain::Date{15, 9, 2026},
                domain::TimeSlot{
                    std::chrono::minutes{20 * 60},
                    std::chrono::minutes{21 * 60}},
                domain::ReminderType::Assignment,
                domain::ReminderRecurrence::Daily}};

        // Execução
        std::vector<std::uint64_t> saved_ids;
        saved_ids.reserve(reminders.size());

        for (const auto& reminder : reminders)
        {
            const auto saved_id = repository.save(reminder);

            VP_EXPECT(
                saved_id != 0,
                "save() deve devolver o id gerado pelo banco");

            saved_ids.push_back(saved_id);
        }

        // Verificação: todos os valores de ReminderType e ReminderRecurrence
        // são preservados após a persistência.
        for (std::size_t index = 0; index < reminders.size(); ++index)
        {
            const auto& expected = reminders[index];
            const auto stored = repository.find_by_id(saved_ids[index]);

            VP_EXPECT(
                stored.has_value(),
                "find_by_id() deve retornar cada lembrete salvo");

            expect_reminder(
                *stored,
                saved_ids[index],
                expected.description(),
                expected.category(),
                expected.date(),
                expected.time_slot().start(),
                expected.time_slot().end(),
                expected.type(),
                expected.recurrence());
        }

        VP_EXPECT(
            !repository.find_by_id(900000000000049999ULL).has_value(),
            "find_by_id() deve retornar nullopt para um id inexistente");

        const auto all_reminders = repository.find_all();

        for (const auto id : saved_ids)
        {
            const auto found = std::any_of(
                all_reminders.begin(),
                all_reminders.end(),
                [id](const domain::Reminder& reminder)
                {
                    return reminder.id() == id;
                });

            VP_EXPECT(
                found,
                "find_all() deve incluir todos os lembretes salvos pelo teste");
        }

        // Regressão da issue #90: save() sempre insere. Uma entidade que
        // carrega um id já existente não pode sobrescrever aquele registro.
        const auto count_before_colliding_save = repository.find_all().size();

        const domain::Reminder colliding{
            saved_ids[0],
            "Não deve sobrescrever",
            domain::Category::Health,
            domain::Date{29, 2, 2024},
            domain::TimeSlot{
                std::chrono::minutes{1439},
                std::chrono::minutes{1440}},
            domain::ReminderType::Exercise,
            domain::ReminderRecurrence::Daily};

        const auto colliding_id = repository.save(colliding);
        saved_ids.push_back(colliding_id);

        VP_EXPECT(
            colliding_id != saved_ids[0],
            "save() deve gerar um id novo, mesmo quando a entidade traz um id existente");
        VP_EXPECT(
            repository.find_all().size() == count_before_colliding_save + 1,
            "save() deve inserir uma linha nova, nunca substituir uma existente");
        VP_EXPECT(
            repository.find_by_id(saved_ids[0])->description() == "Reunião do projeto",
            "save() de uma entidade com id existente não pode alterar aquele registro");

        // update() é a operação que substitui.
        const domain::Reminder updated{
            saved_ids[0],
            "Lembrete atualizado",
            domain::Category::College,
            domain::Date{29, 2, 2024},
            domain::TimeSlot{
                std::chrono::minutes{1439},
                std::chrono::minutes{1440}},
            domain::ReminderType::Assignment,
            domain::ReminderRecurrence::Monthly};

        repository.update(updated);

        const auto reloaded = repository.find_by_id(saved_ids[0]);

        VP_EXPECT(
            reloaded.has_value(),
            "o lembrete atualizado deve continuar disponível para consulta");

        expect_reminder(
            *reloaded,
            saved_ids[0],
            updated.description(),
            updated.category(),
            updated.date(),
            updated.time_slot().start(),
            updated.time_slot().end(),
            updated.type(),
            updated.recurrence());

        const auto after_update = repository.find_all();
        const auto matching_id_count = std::count_if(
            after_update.begin(),
            after_update.end(),
            [&saved_ids](const domain::Reminder& reminder)
            {
                return reminder.id() == saved_ids[0];
            });

        VP_EXPECT(
            matching_id_count == 1,
            "update() deve manter exatamente um registro com aquele id");

        // update() de um id inexistente não pode criar nada.
        const auto count_before_unknown_update = repository.find_all().size();

        const domain::Reminder unknown{
            900000000000049999ULL,
            "Inexistente",
            domain::Category::Leisure,
            domain::Date{1, 3, 2026},
            domain::TimeSlot{
                std::chrono::minutes{8 * 60},
                std::chrono::minutes{9 * 60}},
            domain::ReminderType::Shopping,
            domain::ReminderRecurrence::Once};

        repository.update(unknown);

        VP_EXPECT(
            repository.find_all().size() == count_before_unknown_update,
            "update() de um id inexistente não deve inserir nada");
        VP_EXPECT(
            !repository.find_by_id(900000000000049999ULL).has_value(),
            "update() não pode criar um lembrete");

        // Execução e verificação: remover um id existente ou inexistente deve
        // ser seguro.
        repository.remove(saved_ids[0]);
        VP_EXPECT(
            !repository.find_by_id(saved_ids[0]).has_value(),
            "remove() deve excluir um lembrete existente");

        repository.remove(saved_ids[0]);
        VP_EXPECT(
            !repository.find_by_id(saved_ids[0]).has_value(),
            "remove() de um id inexistente não deve causar alteração");

        remove_saved_reminders(repository, saved_ids);
        database.shutdown();

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
