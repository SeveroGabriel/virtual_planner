#include "virtual_planner/application/reminder/list_reminders_use_case.hpp"

#include "support/expect.hpp"
#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"

#include <chrono>
#include <stdexcept>

using namespace virtual_planner;

namespace {

domain::Reminder criar_lembrete(
    std::uint64_t id,
    domain::Date data,
    domain::ReminderType tipo,
    domain::ReminderRecurrence recorrencia,
    std::chrono::hours inicio = std::chrono::hours{9})
{
    return domain::Reminder{
        id,
        "Lembrete",
        domain::Category::Study,
        data,
        domain::TimeSlot{inicio, inicio + std::chrono::hours{1}},
        tipo,
        recorrencia};
}

application::ListRemindersRequest janela(
    domain::Date inicio,
    domain::Date fim,
    std::optional<domain::ReminderType> tipo = std::nullopt,
    std::optional<domain::ReminderRecurrence> recorrencia = std::nullopt)
{
    return application::ListRemindersRequest{inicio, fim, tipo, recorrencia};
}

} // namespace

int main()
{
    {
        persistence::InMemoryReminderRepository repositorio;
        application::ListRemindersUseCase listar(repositorio);

        VP_EXPECT(
            listar.execute(janela(domain::Date{1, 8, 2026}, domain::Date{31, 8, 2026})).empty(),
            "um repositório vazio deve produzir uma lista de ocorrências vazia");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        repositorio.save(criar_lembrete(1, domain::Date{10, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once));
        repositorio.save(criar_lembrete(2, domain::Date{10, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Weekly));
        repositorio.save(criar_lembrete(3, domain::Date{10, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Once));
        application::ListRemindersUseCase listar(repositorio);

        const auto por_tipo = listar.execute(janela(
            domain::Date{10, 8, 2026}, domain::Date{10, 8, 2026}, domain::ReminderType::Meeting));
        VP_EXPECT(por_tipo.size() == 2, "o filtro por tipo deve manter os lembretes correspondentes");

        const auto por_recorrencia = listar.execute(janela(
            domain::Date{10, 8, 2026}, domain::Date{17, 8, 2026}, std::nullopt, domain::ReminderRecurrence::Once));
        VP_EXPECT(por_recorrencia.size() == 2, "o filtro por recorrência deve manter os lembretes correspondentes");

        const auto combinados = listar.execute(janela(
            domain::Date{10, 8, 2026}, domain::Date{17, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Weekly));
        VP_EXPECT(combinados.size() == 2, "os filtros por tipo e recorrência devem usar semântica AND");
        VP_EXPECT(combinados[0].reminder.id() == 2 && combinados[1].reminder.id() == 2, "os filtros combinados devem excluir lembretes não correspondentes");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        repositorio.save(criar_lembrete(10, domain::Date{15, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once));
        application::ListRemindersUseCase listar(repositorio);

        const auto dentro = listar.execute(janela(domain::Date{15, 8, 2026}, domain::Date{15, 8, 2026}));
        const auto fora = listar.execute(janela(domain::Date{16, 8, 2026}, domain::Date{20, 8, 2026}));

        VP_EXPECT(dentro.size() == 1, "a recorrência única deve ocorrer exatamente uma vez na data-base");
        VP_EXPECT(fora.empty(), "a recorrência única não deve se repetir após a data-base");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        repositorio.save(criar_lembrete(20, domain::Date{10, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Daily));
        application::ListRemindersUseCase listar(repositorio);

        const auto ocorrencias = listar.execute(janela(domain::Date{8, 8, 2026}, domain::Date{12, 8, 2026}));

        VP_EXPECT(ocorrencias.size() == 3, "a recorrência diária deve incluir a data-base e cada dia seguinte dentro da janela");
        VP_EXPECT((ocorrencias.front().occurrence_date == domain::Date{10, 8, 2026}), "a recorrência não deve gerar ocorrências anteriores à data-base");
        VP_EXPECT((ocorrencias.back().occurrence_date == domain::Date{12, 8, 2026}), "o fim da janela deve ser inclusivo");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        repositorio.save(criar_lembrete(30, domain::Date{3, 8, 2026}, domain::ReminderType::Meeting, domain::ReminderRecurrence::Weekly));
        application::ListRemindersUseCase listar(repositorio);

        const auto ocorrencias = listar.execute(janela(domain::Date{9, 8, 2026}, domain::Date{24, 8, 2026}));

        VP_EXPECT(ocorrencias.size() == 3, "o lembrete semanal iniciado antes da janela deve gerar ocorrências dentro dela");
        VP_EXPECT((ocorrencias[0].occurrence_date == domain::Date{10, 8, 2026}), "a ocorrência semanal deve estar sete dias após a data-base");
        VP_EXPECT((ocorrencias[1].occurrence_date == domain::Date{17, 8, 2026}), "as ocorrências semanais devem permanecer separadas por sete dias");
        VP_EXPECT((ocorrencias[2].occurrence_date == domain::Date{24, 8, 2026}), "a ocorrência semanal no limite final inclusivo deve aparecer");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        repositorio.save(criar_lembrete(40, domain::Date{31, 1, 2026}, domain::ReminderType::Shopping, domain::ReminderRecurrence::Monthly));
        application::ListRemindersUseCase listar(repositorio);

        const auto ocorrencias = listar.execute(janela(domain::Date{31, 1, 2026}, domain::Date{30, 4, 2026}));

        VP_EXPECT(ocorrencias.size() == 4, "a recorrência mensal deve produzir uma ocorrência por mês");
        VP_EXPECT((ocorrencias[0].occurrence_date == domain::Date{31, 1, 2026}), "a recorrência mensal deve incluir sua data-base");
        VP_EXPECT((ocorrencias[1].occurrence_date == domain::Date{28, 2, 2026}), "a recorrência mensal deve ajustar para o último dia válido");
        VP_EXPECT((ocorrencias[2].occurrence_date == domain::Date{31, 3, 2026}), "a recorrência mensal deve retornar ao dia-âncora original");
        VP_EXPECT((ocorrencias[3].occurrence_date == domain::Date{30, 4, 2026}), "a recorrência mensal deve ajustar cada mês curto de forma independente");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        repositorio.save(criar_lembrete(50, domain::Date{31, 1, 2024}, domain::ReminderType::Assignment, domain::ReminderRecurrence::Monthly));
        application::ListRemindersUseCase listar(repositorio);

        const auto ocorrencias = listar.execute(janela(domain::Date{29, 2, 2024}, domain::Date{29, 2, 2024}));
        VP_EXPECT(ocorrencias.size() == 1, "uma janela de um dia deve incluir uma ocorrência correspondente");
        VP_EXPECT((ocorrencias.front().occurrence_date == domain::Date{29, 2, 2024}), "a recorrência mensal deve respeitar o dia bissexto ao ajustar a data");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        repositorio.save(criar_lembrete(2, domain::Date{20, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once, std::chrono::hours{9}));
        repositorio.save(criar_lembrete(3, domain::Date{20, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once, std::chrono::hours{8}));
        repositorio.save(criar_lembrete(1, domain::Date{20, 8, 2026}, domain::ReminderType::Study, domain::ReminderRecurrence::Once, std::chrono::hours{9}));
        application::ListRemindersUseCase listar(repositorio);

        const auto tamanho_antes = repositorio.find_all().size();
        const auto ocorrencias = listar.execute(janela(domain::Date{20, 8, 2026}, domain::Date{20, 8, 2026}));

        VP_EXPECT(ocorrencias.size() == 3, "uma janela inclusiva de um dia deve incluir todas as ocorrências correspondentes");
        VP_EXPECT(ocorrencias[0].reminder.id() == 3, "os resultados devem ser ordenados pelo horário inicial após a data");
        VP_EXPECT(ocorrencias[1].reminder.id() == 1, "os resultados devem usar o ID como desempate final");
        VP_EXPECT(ocorrencias[2].reminder.id() == 2, "os resultados devem usar o ID como desempate final");
        VP_EXPECT(repositorio.find_all().size() == tamanho_antes, "a expansão não deve adicionar entidades ao repositório");
        VP_EXPECT((repositorio.find_by_id(1)->date() == domain::Date{20, 8, 2026}), "a expansão deve preservar as datas-base persistidas");
    }

    {
        persistence::InMemoryReminderRepository repositorio;
        application::ListRemindersUseCase listar(repositorio);
        bool invertida_rejeitada = false;

        try
        {
            const auto nao_utilizado = listar.execute(
                janela(domain::Date{2, 8, 2026}, domain::Date{1, 8, 2026}));
            (void)nao_utilizado;
        }
        catch (const std::invalid_argument&)
        {
            invertida_rejeitada = true;
        }

        VP_EXPECT(invertida_rejeitada, "uma janela de datas invertida deve ser rejeitada");
    }

    return 0;
}
