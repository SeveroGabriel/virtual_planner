#include "virtual_planner/domain/enums/reminder_recurrence.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(ReminderRecurrence recurrence)
{
    switch (recurrence)
    {
        case ReminderRecurrence::Once:
            return "Once";

        case ReminderRecurrence::Daily:
            return "Daily";

        case ReminderRecurrence::Weekly:
            return "Weekly";

        case ReminderRecurrence::Monthly:
            return "Monthly";
    }

    throw std::invalid_argument("Invalid ReminderRecurrence");
}

ReminderRecurrence reminder_recurrence_from_string(std::string_view value)
{
    if (value == "Once") return ReminderRecurrence::Once;
    if (value == "Daily") return ReminderRecurrence::Daily;
    if (value == "Weekly") return ReminderRecurrence::Weekly;
    if (value == "Monthly") return ReminderRecurrence::Monthly;

    throw std::invalid_argument("Invalid ReminderRecurrence");
}

}  // namespace virtual_planner::domain
           