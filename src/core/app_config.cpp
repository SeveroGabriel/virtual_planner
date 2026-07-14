#include "virtual_planner/core/app_config.hpp"

#include "virtual_planner/shared/errors.hpp"

#include <utility>

namespace virtual_planner::core
{

  ExecutionProfile parse_execution_profile(std::string_view value)
  {
    if (value == "development")
    {
      return ExecutionProfile::Development;
    }
    if (value == "test")
    {
      return ExecutionProfile::Test;
    }
    if (value == "production")
    {
      return ExecutionProfile::Production;
    }

    throw shared::ConfigError("unknown execution profile: " + std::string(value));
  }

  std::string_view to_string(ExecutionProfile profile)
  {
    switch (profile)
    {
    case ExecutionProfile::Development:
      return "development";
    case ExecutionProfile::Test:
      return "test";
    case ExecutionProfile::Production:
      return "production";
    }

    return "development";
  }

  AppConfig::AppConfig()
      : AppConfig("virtual-planner", ExecutionProfile::Development) {}

  AppConfig::AppConfig(std::string app_name, ExecutionProfile profile)
      : app_name_(std::move(app_name)), profile_(profile) {}

  const std::string &AppConfig::app_name() const noexcept { return app_name_; }

  ExecutionProfile AppConfig::profile() const noexcept { return profile_; }

  void AppConfig::set(std::string key, std::string value)
  {
    values_.insert_or_assign(std::move(key), std::move(value));
  }

  std::optional<std::string> AppConfig::get(std::string_view key) const
  {
    const auto found = values_.find(std::string(key));
    if (found == values_.end())
    {
      return std::nullopt;
    }

    return found->second;
  }

  std::string AppConfig::get_or(std::string_view key, std::string fallback) const
  {
    const auto value = get(key);
    if (!value.has_value())
    {
      return fallback;
    }

    return *value;
  }

}
