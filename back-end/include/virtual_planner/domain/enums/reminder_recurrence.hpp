#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {

enum class ReminderRecurrence
{
    Once,
    Daily,
    Weekly,
    Monthly
};

std::string to_string(ReminderRecurrence value);

ReminderRecurrence reminder_recurrence_from_string(std::string_view value);

} // namespace virtual_planner::domain