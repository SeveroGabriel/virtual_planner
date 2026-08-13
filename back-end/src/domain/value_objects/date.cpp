#include "virtual_planner/domain/value_objects/date.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace virtual_planner::domain {

Date::Date(std::uint32_t day,
           std::uint32_t month,
           std::uint32_t year)
    : day_(day),
      month_(month),
      year_(year)
{
    if (month == 0 || month > 12)
    {
        throw std::invalid_argument("Invalid month.");
    }

    if (year < 1900)
    {
        throw std::invalid_argument("Invalid year.");
    }

    if (day == 0 || day > days_in_month(month, year))
    {
        throw std::invalid_argument("Invalid day for the given month and year.");
    }
}
    
std::uint32_t Date::day() const
{
    return day_;
}

std::uint32_t Date::month() const
{
    return month_;
}

std::uint32_t Date::year() const
{
    return year_;
}

std::string Date::to_string() const
{
    std::ostringstream stream;

    stream << std::setfill('0')
           << std::setw(2) << day_
           << "/"
           << std::setw(2) << month_
           << "/"
           << year_;

    return stream.str();
}

bool Date::operator==(const Date& other) const
{
    return day_ == other.day_
        && month_ == other.month_
        && year_ == other.year_;
}

bool Date::operator!=(const Date& other) const
{
    return !(*this == other);
}

bool Date::operator<(const Date& other) const
{
    if (year_ != other.year_)
        return year_ < other.year_;

    if (month_ != other.month_)
        return month_ < other.month_;

    return day_ < other.day_;
}
// ano bissexto - Gregorian calendar rule
bool Date::is_leap_year(std::uint32_t year)
{
    return (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);
}

std::uint32_t Date::days_in_month(std::uint32_t month,
                                  std::uint32_t year)
{
    switch (month)
    {
        case 2:
            return is_leap_year(year) ? 29 : 28;

        case 4:
        case 6:
        case 9:
        case 11:
            return 30;

        default:
            return 31;
    }
}

bool Date::operator>(const Date& other) const
{
    return other < *this;
}

bool Date::operator<=(const Date& other) const
{
    return !(*this > other);
}

bool Date::operator>=(const Date& other) const
{
    return !(*this < other);
}

} // namespace virtual_planner::domain