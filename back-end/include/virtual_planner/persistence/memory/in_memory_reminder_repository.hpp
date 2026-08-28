#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

#include "virtual_planner/persistence/reminder_repository.hpp"

namespace virtual_planner::persistence {

// Repositorio de Reminder em memoria.
//
// save gera o id e insere; update substitui quem ja tem o mesmo id. Espelha
// InMemoryGoalRepository (issue #90) — antes save fazia upsert e era possivel
// sobrescrever um lembrete existente sem querer.
//
// Nao e thread-safe: o vector interno nao tem lock nenhum. O chamador deve
// serializar o acesso concorrente.
class InMemoryReminderRepository final : public ReminderRepository
{
public:
    std::uint64_t save(const domain::Reminder& reminder) override
    {
        const auto id = next_id_++;

        // Reconstroi campo a campo porque Reminder::id_ e privado sem setter
        // e o id gerado aqui precisa sobrescrever o que veio na entidade.
        // Mesma razao do InMemoryGoalRepository.
        reminders_.emplace_back(
            id,
            reminder.description(),
            reminder.category(),
            reminder.date(),
            reminder.time_slot(),
            reminder.type(),
            reminder.recurrence());

        return id;
    }

    void update(const domain::Reminder& reminder) override
    {
        for (auto& current : reminders_)
        {
            if (current.id() == reminder.id())
            {
                current = reminder;
                return;
            }
        }
    }

    std::optional<domain::Reminder> find_by_id(std::uint64_t id) override
    {
        for (const auto& reminder : reminders_)
        {
            if (reminder.id() == id)
            {
                return reminder;
            }
        }

        return std::nullopt;
    }

    std::vector<domain::Reminder> find_all() override
    {
        return reminders_;
    }

    void remove(std::uint64_t id) override
    {
        reminders_.erase(
            std::remove_if(
                reminders_.begin(),
                reminders_.end(),
                [id](const domain::Reminder& reminder)
                {
                    return reminder.id() == id;
                }),
            reminders_.end());
    }

private:
    std::vector<domain::Reminder> reminders_;
    std::uint64_t next_id_{1};
};

} // namespace virtual_planner::persistence
