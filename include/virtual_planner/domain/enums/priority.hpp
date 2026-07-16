#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {

enum class Priority
{
    Low,
    Medium,
    High
};

std::string to_string(Priority value);

Priority priority_from_string(std::string_view value);

} // namespace virtual_planner::domain