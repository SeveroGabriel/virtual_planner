#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

#include "virtual_planner/persistence/user_repository.hpp"

namespace virtual_planner::persistence {

// Repositorio de User em memoria.
//
// UserRepository nao expoe update, entao save faz upsert: substitui quem ja
// tem o mesmo id e insere caso contrario.
//
// Nao e thread-safe: o vector interno nao tem lock nenhum. O chamador deve
// serializar o acesso concorrente.
class InMemoryUserRepository final : public UserRepository
{
public:
    void save(const domain::User& user) override
    {
        for (auto& current : users_)
        {
            if (current.id() == user.id())
            {
                current = user;
                return;
            }
        }

        users_.push_back(user);
    }

    std::optional<domain::User> find_by_id(std::uint64_t id) override
    {
        for (const auto& user : users_)
        {
            if (user.id() == id)
            {
                return user;
            }
        }

        return std::nullopt;
    }

    std::vector<domain::User> find_all() override
    {
        return users_;
    }

    void remove(std::uint64_t id) override
    {
        users_.erase(
            std::remove_if(
                users_.begin(),
                users_.end(),
                [id](const domain::User& user)
                {
                    return user.id() == id;
                }),
            users_.end());
    }

private:
    std::vector<domain::User> users_;
};

} // namespace virtual_planner::persistence
