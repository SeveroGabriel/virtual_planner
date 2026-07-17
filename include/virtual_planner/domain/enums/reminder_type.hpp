#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {

enum class ReminderType
{
    Meeting,
    PhoneCall,
    Shopping,
    Study,
    Exercise,
    Assignment
};

std::string to_string(ReminderType value);

ReminderType reminder_type_from_string(std::string_view value);

} // namespace virtual_planner::domain