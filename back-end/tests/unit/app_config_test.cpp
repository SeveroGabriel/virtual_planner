#include "virtual_planner/core/app_config.hpp"
#include "virtual_planner/shared/errors.hpp"

#include "support/expect.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

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

    VP_EXPECT(config.app_name() == "planner", "app name should be preserved");
    VP_EXPECT(config.profile() == ExecutionProfile::Test, "profile should be preserved");
    VP_EXPECT(config.get("database.host") == "localhost", "stored value should be readable");
    VP_EXPECT(config.get_or("missing", "fallback") == "fallback", "fallback should be returned");
    VP_EXPECT(parse_execution_profile("production") == ExecutionProfile::Production,
           "production profile should parse");
    VP_EXPECT(to_string(ExecutionProfile::Development) == "development",
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

    VP_EXPECT(rejected_unknown_profile, "unknown profile should be rejected");
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
