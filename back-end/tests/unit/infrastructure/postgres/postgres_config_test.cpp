#include "virtual_planner/core/app_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
#include "virtual_planner/shared/errors.hpp"

#include "support/expect.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

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
    VP_EXPECT(postgres_config.host() == "localhost", "host should be preserved");
    VP_EXPECT(postgres_config.port() == 5432, "port should be parsed");
    VP_EXPECT(postgres_config.database() == "virtual_planner_test", "database should be preserved");
    VP_EXPECT(connection_string.find("secret-password") != std::string::npos,
           "connection string should contain the real password");
    VP_EXPECT(masked_connection_string.find("secret-password") == std::string::npos,
           "masked connection string should not expose the password");
    VP_EXPECT(masked_connection_string.find("password='***'") != std::string::npos,
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
    VP_EXPECT(missing_password_rejected, "missing password should be rejected");

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
    VP_EXPECT(invalid_port_rejected, "invalid port should be rejected");

    // Um mesmo conjunto de valores, mudando so o perfil: e o perfil que decide.
    const auto build = [](ExecutionProfile profile,
                          const std::string &password,
                          const std::string &sslmode)
    {
      AppConfig config("planner", profile);
      config.set("postgres.host", "localhost");
      config.set("postgres.port", "5432");
      config.set("postgres.database", "virtual_planner");
      config.set("postgres.user", "planner");
      config.set("postgres.password", password);
      config.set("postgres.sslmode", sslmode);
      return PostgresConfig::from_app_config(config);
    };

    const auto rejects = [](const PostgresConfig &config)
    {
      try
      {
        config.validate();
        return false;
      }
      catch (const virtual_planner::shared::ConfigError &)
      {
        return true;
      }
    };

    // Arrange / Act / Assert: senha publicada no repositorio, em producao.
    VP_EXPECT(rejects(build(ExecutionProfile::Production, "change-me", "require")),
              "production should refuse the password published in the repository");
    VP_EXPECT(rejects(build(ExecutionProfile::Production, "postgres", "require")),
              "production should refuse the historical PostgreSQL default password");

    // Em desenvolvimento a mesma senha passa: e o que faz o compose local subir.
    VP_EXPECT(!rejects(build(ExecutionProfile::Development, "change-me", "disable")),
              "development should still accept the local development password");
    VP_EXPECT(!rejects(build(ExecutionProfile::Test, "change-me", "disable")),
              "test should still accept the local development password");

    // Arrange / Act / Assert: trafego em texto claro, em producao.
    VP_EXPECT(rejects(build(ExecutionProfile::Production, "uma-senha-real", "disable")),
              "production should refuse sslmode=disable");

    // Senha real e TLS ligado: e a unica combinacao que producao aceita.
    VP_EXPECT(!rejects(build(ExecutionProfile::Production, "uma-senha-real", "require")),
              "production should accept a real password over TLS");
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
