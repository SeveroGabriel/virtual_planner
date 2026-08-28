#include "virtual_planner/api/json/goal_json.hpp"

#include "virtual_planner/api/json/shared_json.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>

namespace virtual_planner::api::json {

namespace {

const nlohmann::json& required_field(const nlohmann::json& value,
                                     const char* field)
{
    if (!value.contains(field))
    {
        throw std::invalid_argument(
            std::string{"Goal requires the field \""} + field + "\".");
    }

    return value.at(field);
}

std::uint64_t read_id(const nlohmann::json& value)
{
    const nlohmann::json& id = required_field(value, "id");

    if (!id.is_number_unsigned())
    {
        throw std::invalid_argument(
            "Goal field \"id\" must be an unsigned integer.");
    }

    return id.get<std::uint64_t>();
}

std::string read_description(const nlohmann::json& value)
{
    const nlohmann::json& description = required_field(value, "description");

    if (!description.is_string())
    {
        throw std::invalid_argument(
            "Goal field \"description\" must be a string.");
    }

    return description.get<std::string>();
}

} // namespace

nlohmann::json to_json(const domain::Goal& goal)
{
    return nlohmann::json{
        {"id", goal.id()},
        {"description", goal.description()},
        {"category", to_json(goal.category())},
        {"status", to_json(goal.status())},
        {"period", to_json(goal.period())},
        {"reference_date", to_json(goal.reference_date())},
    };
}

domain::Goal goal_from_json(const nlohmann::json& value)
{
    if (!value.is_object())
    {
        throw std::invalid_argument("Goal must be a JSON object.");
    }

    return domain::Goal{
        read_id(value),
        read_description(value),
        category_from_json(required_field(value, "category")),
        goal_status_from_json(required_field(value, "status")),
        goal_period_from_json(required_field(value, "period")),
        date_from_json(required_field(value, "reference_date")),
    };
}

} // namespace virtual_planner::api::json
