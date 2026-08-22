#pragma once

#include <cstdint>
#include <string>

namespace virtual_planner::domain {

class User
{
public:
    User(
        std::uint64_t id,
        std::string name,
        std::string email
    );

    [[nodiscard]] std::uint64_t id() const;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] const std::string& email() const;

    void update_name(std::string new_name);

    void update_email(std::string new_email);

private:
    std::uint64_t id_;

    std::string name_;

    std::string email_;

    void validate_name(const std::string& name) const;

    void validate_email(const std::string& email) const;
};

} // namespace virtual_planner::domain