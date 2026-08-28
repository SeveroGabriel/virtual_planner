#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "virtual_planner/domain/entities/reminder.hpp"

namespace virtual_planner::persistence {

// Contrato alinhado ao de GoalRepository (issue #90).
//
// Antes, save aceitava um id vindo do chamador e fazia upsert: qualquer ponto
// de chamada que esquecesse de checar find_by_id antes sobrescrevia um
// registro existente em silencio. A guarda que o #87 adicionou protegia um
// unico call site, nao o contrato.
//
// Agora o id e do repositorio, e criar e atualizar sao operacoes distintas.
// Nao existe mais como sobrescrever um lembrete por engano: save so insere.
class ReminderRepository
{
public:
    virtual ~ReminderRepository() = default;

    // Insere um lembrete novo, gera o id e o devolve. O id que vier na
    // entidade e ignorado.
    virtual std::uint64_t save(const domain::Reminder& reminder) = 0;

    // Atualiza o lembrete de mesmo id. Nao cria nada quando o id nao existe.
    virtual void update(const domain::Reminder& reminder) = 0;

    virtual std::optional<domain::Reminder> find_by_id(
        std::uint64_t id) = 0;

    virtual std::vector<domain::Reminder> find_all() = 0;

    virtual void remove(std::uint64_t id) = 0;
};

} // namespace virtual_planner::persistence
