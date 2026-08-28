#include "virtual_planner/domain/text.hpp"

namespace virtual_planner::domain {

bool is_blank(std::string_view value)
{
    return value.find_first_not_of(" \t\n\r\f\v") == std::string_view::npos;
}

} // namespace virtual_planner::domain
