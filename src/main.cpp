#include "virtual_planner/infrastructure/config/environment_config_loader.hpp"

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)
#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_database.hpp"
#endif

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{

  bool postgres_enabled()
  {
    const char *value = std::getenv("VP_USE_POSTGRES");
    return value != nullptr && std::string(value) == "true";
  }

}

int main() {
  const virtual_planner::infrastructure::config::EnvironmentConfigLoader loader;
  const auto config = loader.load();

  std::cout << config.app_name() << " running in "
            << virtual_planner::core::to_string(config.profile()) << " profile\n";

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)
  if (postgres_enabled())
  {
    virtual_planner::infrastructure::postgres::PostgresDatabase database(
        virtual_planner::infrastructure::postgres::PostgresConfig::from_environment());
    database.connect();
    std::cout << "PostgreSQL connection established\n";
  }
#else
  if (postgres_enabled())
  {
    std::cerr << "PostgreSQL support was not compiled. Rebuild with "
              << "-DVIRTUAL_PLANNER_WITH_POSTGRES=ON\n";
    return 1;
  }
#endif

  return 0;
}
