#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::domain {

enum class Category {
    College,
    Work,
    Health,
    Leisure,
    PersonalProjects,
    Study
};

std::string to_string(Category value);

Category category_from_string(std::string_view value);

} // namespace virtual_planner::domain