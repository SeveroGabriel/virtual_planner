#pragma once

#include <nlohmann/json.hpp>

#include "virtual_planner/domain/entities/goal.hpp"

namespace virtual_planner::api::json {

nlohmann::json to_json(const domain::Goal& goal);

domain::Goal goal_from_json(const nlohmann::json& value);

} // namespace virtual_planner::api::json
