#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

#include "virtual_planner/persistence/reminder_repository.hpp"

namespace virtual_planner::persistence {

// Repositorio de Reminder em memoria.
//
// ReminderRepository nao expoe update, entao save faz upsert: substitui quem
// ja tem o mesmo id e insere caso contrario.
//
// Nao e thread-safe: o vector interno nao tem lock nenhum. O chamador deve
// serializar o acesso concorrente.
class InMemoryReminderRepository final : public ReminderRepository
{
public:
    void save(const domain::Reminder& reminder) override
    {
        for (auto& current : reminders_)
        {
            if (current.id() == reminder.id())
            {
                current = reminder;
                return;
            }
        }

        reminders_.push_back(reminder);
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
};

} // namespace virtual_planner::persistence
