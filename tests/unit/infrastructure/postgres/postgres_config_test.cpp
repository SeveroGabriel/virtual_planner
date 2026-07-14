#include "virtual_planner/core/app_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
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
  using virtual_planner::infrastructure::postgres::PostgresConfig;

  try
  {
    // Arrange
    AppConfig config("planner", ExecutionProfile::Test);
    config.set("postgres.host", "localhost");
    config.set("postgres.port", "5432");
    config.set("postgres.database", "virtual_planner_test");
    config.set("postgres.user", "planner");
    config.set("postgres.password", "secret-password");
    config.set("postgres.sslmode", "disable");
    config.set("postgres.connect_timeout", "7");
    config.set("postgres.application_name", "virtual-planner-test");

    // Act
    const auto postgres_config = PostgresConfig::from_app_config(config);
    const auto connection_string = postgres_config.connection_string();
    const auto masked_connection_string = postgres_config.masked_connection_string();

    // Assert
    expect(postgres_config.host() == "localhost", "host should be preserved");
    expect(postgres_config.port() == 5432, "port should be parsed");
    expect(postgres_config.database() == "virtual_planner_test", "database should be preserved");
    expect(connection_string.find("secret-password") != std::string::npos,
           "connection string should contain the real password");
    expect(masked_connection_string.find("secret-password") == std::string::npos,
           "masked connection string should not expose the password");
    expect(masked_connection_string.find("password='***'") != std::string::npos,
           "masked connection string should show a placeholder");

    // Arrange
    AppConfig missing_password("planner", ExecutionProfile::Test);
    missing_password.set("postgres.database", "virtual_planner_test");
    missing_password.set("postgres.user", "planner");

    // Act
    bool missing_password_rejected = false;
    try
    {
      (void)PostgresConfig::from_app_config(missing_password);
    }
    catch (const virtual_planner::shared::ConfigError &)
    {
      missing_password_rejected = true;
    }

    // Assert
    expect(missing_password_rejected, "missing password should be rejected");

    // Arrange
    AppConfig invalid_port("planner", ExecutionProfile::Test);
    invalid_port.set("postgres.port", "999999");
    invalid_port.set("postgres.database", "virtual_planner_test");
    invalid_port.set("postgres.user", "planner");
    invalid_port.set("postgres.password", "secret-password");

    // Act
    bool invalid_port_rejected = false;
    try
    {
      (void)PostgresConfig::from_app_config(invalid_port);
    }
    catch (const virtual_planner::shared::ConfigError &)
    {
      invalid_port_rejected = true;
    }

    // Assert
    expect(invalid_port_rejected, "invalid port should be rejected");
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
