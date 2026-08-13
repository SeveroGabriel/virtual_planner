#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {
    
enum class Shift
{
    Morning,
    Afternoon,
    Evening
};

std::string to_string(Shift value);

Shift shift_from_string(std::string_view value);

} // namespace virtual_planner::domain