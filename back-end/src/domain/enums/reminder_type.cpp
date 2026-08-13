#include "virtual_planner/domain/enums/reminder_type.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(ReminderType reminder_type)
{

    switch (reminder_type)
    {
        case ReminderType::Meeting:
            return "Meeting";

        case ReminderType::PhoneCall:
            return "PhoneCall";

        case ReminderType::Shopping:
            return "Shopping";

        case ReminderType::Study:
            return "Study";

        case ReminderType::Exercise:
            return "Exercise";

        case ReminderType::Assignment:
            return "Assignment";
    }

    throw std::invalid_argument("Invalid ReminderType");
}

ReminderType reminder_type_from_string(std::string_view value)
{
    if (value == "Meeting")   return ReminderType::Meeting;
    if (value == "PhoneCall") return ReminderType::PhoneCall;
    if (value == "Shopping")  return ReminderType::Shopping;
    if (value == "Study")     return ReminderType::Study;
    if (value == "Exercise")  return ReminderType::Exercise;
    if (value == "Assignment") return ReminderType::Assignment;

    throw std::invalid_argument("Invalid ReminderType.");
}

}  // namespace virtual_planner::domain
           