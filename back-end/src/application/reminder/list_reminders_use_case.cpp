#include "virtual_planner/application/reminder/list_reminders_use_case.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>

namespace virtual_planner::application {

namespace {

using Days = std::chrono::sys_days;

Days para_dias_sistema(const domain::Date& data)
{
    return Days{
        std::chrono::year{static_cast<int>(data.year())} /
        std::chrono::month{data.month()} /
        std::chrono::day{data.day()}};
}

domain::Date para_data(const Days valor)
{
    const std::chrono::year_month_day data_calendario{valor};

    return domain::Date{
        static_cast<std::uint32_t>(static_cast<unsigned>(data_calendario.day())),
        static_cast<std::uint32_t>(static_cast<unsigned>(data_calendario.month())),
        static_cast<std::uint32_t>(static_cast<int>(data_calendario.year()))};
}

void adicionar_se_na_janela(
    std::vector<ReminderOccurrence>& ocorrencias,
    const domain::Reminder& lembrete,
    const Days ocorrencia,
    const Days inicio_janela,
    const Days fim_janela)
{
    if (ocorrencia >= inicio_janela && ocorrencia <= fim_janela)
    {
        ocorrencias.push_back(ReminderOccurrence{lembrete, para_data(ocorrencia)});
    }
}

void adicionar_ocorrencias_intervalo_fixo(
    std::vector<ReminderOccurrence>& ocorrencias,
    const domain::Reminder& lembrete,
    const Days inicio_janela,
    const Days fim_janela,
    const std::chrono::days intervalo)
{
    const Days data_base = para_dias_sistema(lembrete.date());

    for (Days ocorrencia = data_base; ocorrencia <= fim_janela; ocorrencia += intervalo)
    {
        adicionar_se_na_janela(
            ocorrencias, lembrete, ocorrencia, inicio_janela, fim_janela);
    }
}

void adicionar_ocorrencias_mensais(
    std::vector<ReminderOccurrence>& ocorrencias,
    const domain::Reminder& lembrete,
    const Days inicio_janela,
    const Days fim_janela)
{
    const auto data_base = std::chrono::year_month_day{para_dias_sistema(lembrete.date())};
    const unsigned dia_ancora = static_cast<unsigned>(data_base.day());

    for (std::chrono::year_month mes = data_base.year() / data_base.month();; mes += std::chrono::months{1})
    {
        const auto data_solicitada = mes / std::chrono::day{dia_ancora};
        const auto data_valida = data_solicitada.ok()
            ? data_solicitada
            : std::chrono::year_month_day{mes / std::chrono::last};
        const Days ocorrencia{data_valida};

        if (ocorrencia > fim_janela)
        {
            break;
        }

        adicionar_se_na_janela(
            ocorrencias, lembrete, ocorrencia, inicio_janela, fim_janela);
    }
}

bool corresponde_aos_filtros(
    const domain::Reminder& lembrete,
    const ListRemindersRequest& requisicao)
{
    return (!requisicao.type.has_value() || lembrete.type() == *requisicao.type) &&
           (!requisicao.recurrence.has_value() ||
            lembrete.recurrence() == *requisicao.recurrence);
}

} // namespace

ListRemindersUseCase::ListRemindersUseCase(
    persistence::ReminderRepository& repository)
    : repositorio_(repository)
{
}

std::vector<ReminderOccurrence> ListRemindersUseCase::execute(
    const ListRemindersRequest& request) const
{
    if (request.start_date > request.end_date)
    {
        throw std::invalid_argument(
            "O início da janela de datas não pode ser posterior ao fim.");
    }

    const Days inicio_janela = para_dias_sistema(request.start_date);
    const Days fim_janela = para_dias_sistema(request.end_date);
    std::vector<ReminderOccurrence> ocorrencias;

    for (const auto& lembrete : repositorio_.find_all())
    {
        if (!corresponde_aos_filtros(lembrete, request))
        {
            continue;
        }

        switch (lembrete.recurrence())
        {
            case domain::ReminderRecurrence::Once:
                adicionar_se_na_janela(
                    ocorrencias,
                    lembrete,
                    para_dias_sistema(lembrete.date()),
                    inicio_janela,
                    fim_janela);
                break;

            case domain::ReminderRecurrence::Daily:
                adicionar_ocorrencias_intervalo_fixo(
                    ocorrencias,
                    lembrete,
                    inicio_janela,
                    fim_janela,
                    std::chrono::days{1});
                break;

            case domain::ReminderRecurrence::Weekly:
                adicionar_ocorrencias_intervalo_fixo(
                    ocorrencias,
                    lembrete,
                    inicio_janela,
                    fim_janela,
                    std::chrono::days{7});
                break;

            case domain::ReminderRecurrence::Monthly:
                adicionar_ocorrencias_mensais(
                    ocorrencias, lembrete, inicio_janela, fim_janela);
                break;
        }
    }

    std::sort(
        ocorrencias.begin(),
        ocorrencias.end(),
        [](const ReminderOccurrence& esquerda, const ReminderOccurrence& direita)
        {
            if (esquerda.occurrence_date != direita.occurrence_date)
            {
                return esquerda.occurrence_date < direita.occurrence_date;
            }

            if (esquerda.reminder.time_slot().start() !=
                direita.reminder.time_slot().start())
            {
                return esquerda.reminder.time_slot().start() <
                       direita.reminder.time_slot().start();
            }

            return esquerda.reminder.id() < direita.reminder.id();
        });

    return ocorrencias;
}

} // namespace virtual_planner::application
