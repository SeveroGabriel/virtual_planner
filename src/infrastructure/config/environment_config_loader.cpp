#include "virtual_planner/infrastructure/config/environment_config_loader.hpp"

#include <cstdlib>
#include <string>
#include <utility>

namespace virtual_planner::infrastructure::config
{

  namespace
  {

    std::string read_environment_or(const char *name, std::string fallback)
    {
      const char *value = std::getenv(name);
      if (value == nullptr)
      {
        return fallback;
      }

      return value;
    }

  }

  core::AppConfig EnvironmentConfigLoader::load() const
  {
    auto app_name = read_environment_or("VP_APP_NAME", "virtual-planner");
    auto profile = core::parse_execution_profile(
        read_environment_or("VP_PROFILE", "development"));

    core::AppConfig config(std::move(app_name), profile);
    config.set("profile", std::string(core::to_string(profile)));
    return config;
  }

} // namespace virtual_planner::infrastructure::config
