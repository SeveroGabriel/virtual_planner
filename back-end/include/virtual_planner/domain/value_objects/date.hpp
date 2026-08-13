#pragma once

#include <cstdint>
#include <string>

namespace virtual_planner::domain {

class Date
{
public:
    Date(std::uint32_t day,
         std::uint32_t month,
         std::uint32_t year);

    [[nodiscard]] std::uint32_t day() const;
    [[nodiscard]] std::uint32_t month() const;
    [[nodiscard]] std::uint32_t year() const;

    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] bool operator==(const Date& other) const;
    [[nodiscard]] bool operator!=(const Date& other) const;
    [[nodiscard]] bool operator<(const Date& other) const;

    [[nodiscard]] bool operator>(const Date& other) const;

    [[nodiscard]] bool operator<=(const Date& other) const;

    [[nodiscard]] bool operator>=(const Date& other) const;

private:
    static bool is_leap_year(std::uint32_t year);

    static std::uint32_t days_in_month(std::uint32_t month,
                                       std::uint32_t year);

    std::uint32_t day_;
    std::uint32_t month_;
    std::uint32_t year_;
};

} // namespace virtual_planner::domain