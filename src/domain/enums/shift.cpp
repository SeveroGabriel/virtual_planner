#include "virtual_planner/domain/enums/shift.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(Shift shift)
{
    switch (shift)
    {
        case Shift::Morning:
            return "Morning";

        case Shift::Afternoon:
            return "Afternoon";

        case Shift::Evening:
            return "Evening";

    }

    throw std::invalid_argument("Invalid Shift");
}

Shift shift_from_string(std::string_view value)
{
    if (value == "Morning") return Shift::Morning;
    if (value == "Afternoon") return Shift::Afternoon;
    if (value == "Evening") return Shift::Evening;

    throw std::invalid_argument("Invalid ReminderType");
}

}  // namespace virtual_planner::domain
           