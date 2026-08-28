#include "virtual_planner/domain/entities/user.hpp"

#include "virtual_planner/domain/text.hpp"

#include <stdexcept>
#include <utility>

namespace virtual_planner::domain {
    User::User(std::uint64_t id, std::string name, std::string email)
        : id_(id)
    {
        validate_name(name);
        validate_email(email);
        name_ = std::move(name);
        email_ = std::move(email);
    }

    std::uint64_t User::id() const
    {
        return id_;
    }

    const std::string& User::name() const
    {
        return name_;
    }

    const std::string& User::email() const
    {
        return email_;
    }

    void User::update_name(std::string new_name)
    {
        validate_name(new_name);
        name_ = std::move(new_name);
    }

    void User::update_email(std::string new_email)
    {
        validate_email(new_email);
        email_ = std::move(new_email);
    }

    void User::validate_name(const std::string& name) const
    {
        if (is_blank(name))
        {
            throw std::invalid_argument(
                "User name cannot be empty or blank.");
        }
    }

    void User::validate_email(const std::string& email) const
    {
        if (is_blank(email))
        {
            throw std::invalid_argument(
                "User email cannot be empty or blank.");
        }

        if (email.find('@') == std::string::npos)
        {
            throw std::invalid_argument("Invalid email.");
        }
    }
} // namespace virtual_planner::domain