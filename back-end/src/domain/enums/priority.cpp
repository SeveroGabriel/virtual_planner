#include "virtual_planner/domain/enums/priority.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(Priority priority)
{
    switch (priority)
    {
        case Priority::Low:
            return "Low";

        case Priority::Medium:
            return "Medium";

        case Priority::High:
            return "High";
    }

    throw std::invalid_argument("Invalid Priority");
}

Priority priority_from_string(std::string_view value)
{
    if (value == "Low") return Priority::Low;
    if (value == "Medium") return Priority::Medium;
    if (value == "High") return Priority::High;

    throw std::invalid_argument("Invalid Priority");
}

}  // namespace virtual_planner::domain