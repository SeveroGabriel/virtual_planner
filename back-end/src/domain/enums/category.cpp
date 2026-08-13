#include "virtual_planner/domain/enums/category.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

std::string to_string(Category category)
{
    switch (category)
    {
        case Category::College:
            return "College";

        case Category::Work:
            return "Work";

        case Category::Health:
            return "Health";

        case Category::Leisure:
            return "Leisure";

        case Category::PersonalProjects:
            return "PersonalProjects";

        case Category::Study:
            return "Study";
    }

    throw std::invalid_argument("Invalid Category");
}

Category category_from_string(std::string_view value)
{
    if (value == "College") return Category::College;
    if (value == "Work") return Category::Work;
    if (value == "Health") return Category::Health;
    if (value == "Leisure") return Category::Leisure;
    if (value == "PersonalProjects") return Category::PersonalProjects;
    if (value == "Study") return Category::Study;

    throw std::invalid_argument("Invalid Category");
}

} // namespace virtual_planner::domain