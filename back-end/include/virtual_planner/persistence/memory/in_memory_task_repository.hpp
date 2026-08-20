#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

#include "virtual_planner/persistence/task_repository.hpp"

namespace virtual_planner::persistence {

// Repositorio de Task em memoria.
//
// TaskRepository nao expoe update, entao save faz upsert: substitui quem ja
// tem o mesmo id e insere caso contrario. Sem isso nao existe como alterar
// uma Task ja salva.
//
// Nao e thread-safe: o vector interno nao tem lock nenhum. O chamador deve
// serializar o acesso concorrente.
class InMemoryTaskRepository final : public TaskRepository
{
public:
    void save(const domain::Task& task) override
    {
        for (auto& current : tasks_)
        {
            if (current.id() == task.id())
            {
                current = task;
                return;
            }
        }

        tasks_.push_back(task);
    }

    std::optional<domain::Task> find_by_id(std::uint64_t id) override
    {
        for (const auto& task : tasks_)
        {
            if (task.id() == id)
            {
                return task;
            }
        }

        return std::nullopt;
    }

    std::vector<domain::Task> find_all() override
    {
        return tasks_;
    }

    void remove(std::uint64_t id) override
    {
        tasks_.erase(
            std::remove_if(
                tasks_.begin(),
                tasks_.end(),
                [id](const domain::Task& task)
                {
                    return task.id() == id;
                }),
            tasks_.end());
    }

private:
    std::vector<domain::Task> tasks_;
};

} // namespace virtual_planner::persistence
