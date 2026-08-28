#pragma once

namespace virtual_planner::api::http {

class ApiServer;

// Registra os endpoints de relatorios e dashboard da P-34.
void register_reporting_routes(ApiServer& api);

} // namespace virtual_planner::api::http
