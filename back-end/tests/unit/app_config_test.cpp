#include "virtual_planner/core/app_config.hpp"
#include "virtual_planner/shared/errors.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace
{

  void expect(bool condition, const std::string &message)
  {
    if (!condition)
    {
      throw std::runtime_error(message);
    }
  }

}

int main()
{
  using virtual_planner::core::AppConfig;
  using virtual_planner::core::ExecutionProfile;
  using virtual_planner::core::parse_execution_profile;
  using virtual_planner::core::to_string;

  try
  {
    AppConfig config("planner", ExecutionProfile::Test);
    config.set("database.host", "localhost");

    expect(config.app_name() == "planner", "app name should be preserved");
    expect(config.profile() == ExecutionProfile::Test, "profile should be preserved");
    expect(config.get("database.host") == "localhost", "stored value should be readable");
    expect(config.get_or("missing", "fallback") == "fallback", "fallback should be returned");
    expect(parse_execution_profile("production") == ExecutionProfile::Production,
           "production profile should parse");
    expect(to_string(ExecutionProfile::Development) == "development",
           "development profile should format");

    bool rejected_unknown_profile = false;
    try
    {
      (void)parse_execution_profile("invalid");
    }
    catch (const virtual_planner::shared::ConfigError &)
    {
      rejected_unknown_profile = true;
    }

    expect(rejected_unknown_profile, "unknown profile should be rejected");
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
