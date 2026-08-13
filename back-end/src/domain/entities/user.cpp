#include "virtual_planner/domain/entities/user.hpp"

#include <stdexcept>
#include <utility>

namespace virtual_planner::domain {

User::User(
    std::uint64_t id,
    std::string name,
    std::string email
)
    : id_(id),
      name_(std::move(name)),
      email_(std::move(email))
{
    if (name_.empty())
    {
        throw std::invalid_argument(
            "User name cannot be empty."
        );
    }

    if (email_.empty())
    {
        throw std::invalid_argument(
            "User email cannot be empty."
        );
    }

    if (email_.find('@') == std::string::npos)
    {
        throw std::invalid_argument(
            "Invalid email."
        );
    }
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

void User::update_name(std::string name)
{
    if (name.empty())
    {
        throw std::invalid_argument(
            "User name cannot be empty."
        );
    }

    name_ = std::move(name);
}

void User::update_email(std::string email)
{
    if (email.empty())
    {
        throw std::invalid_argument(
            "User email cannot be empty."
        );
    }

    if (email.find('@') == std::string::npos)
    {
        throw std::invalid_argument(
            "Invalid email."
        );
    }

    email_ = std::move(email);
}

} // namespace virtual_planner::domain