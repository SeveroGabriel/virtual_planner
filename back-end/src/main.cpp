// Composition root da aplicacao.
//
// Este arquivo e o unico lugar que escolhe implementacoes concretas: qual
// repositorio (in-memory ou PostgreSQL), qual banco e com que configuracao o
// servidor HTTP sobe. Nenhuma camada abaixo conhece essa decisao.

#include "virtual_planner/infrastructure/config/environment_config_loader.hpp"
#include "virtual_planner/persistence/database.hpp"
#include "virtual_planner/persistence/memory/repositories.hpp"
#include "virtual_planner/persistence/repository_set.hpp"
#include "virtual_planner/shared/errors.hpp"

#if defined(VIRTUAL_PLANNER_WITH_HTTP)
#include "virtual_planner/api/http/api_server.hpp"
#include "virtual_planner/api/http/server_config.hpp"
#endif

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)
#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_database.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_goal_repository.hpp"
#include "virtual_planner/infrastructure/postgres/postgres_reminder_repository.hpp"

#include <optional>
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
  try
  {
    const virtual_planner::infrastructure::config::EnvironmentConfigLoader loader;
    const auto config = loader.load();

    std::cout << config.app_name() << " running in "
              << virtual_planner::core::to_string(config.profile()) << " profile\n";

    // In-memory e o padrao. Task e User continuam in-memory mesmo com o banco
    // ligado, porque essas duas entidades ainda nao tem adapter PostgreSQL.
    virtual_planner::persistence::InMemoryGoalRepository memory_goals;
    virtual_planner::persistence::InMemoryTaskRepository memory_tasks;
    virtual_planner::persistence::InMemoryReminderRepository memory_reminders;
    virtual_planner::persistence::InMemoryUserRepository memory_users;

    [[maybe_unused]] virtual_planner::persistence::RepositorySet repositories{
        &memory_goals, &memory_tasks, &memory_reminders, &memory_users};

    // Nulo quando a aplicacao roda sem banco: /api/health reporta isso em vez
    // de fingir que ha um banco saudavel.
    [[maybe_unused]] const virtual_planner::persistence::Database *health_database = nullptr;

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)
    // std::optional porque o banco e os repositorios so existem quando
    // VP_USE_POSTGRES=true, mas precisam viver ate o fim do main: o
    // RepositorySet guarda ponteiros para eles.
    std::optional<virtual_planner::infrastructure::postgres::PostgresDatabase> database;
    std::optional<virtual_planner::infrastructure::postgres::PostgresGoalRepository> postgres_goals;
    std::optional<virtual_planner::infrastructure::postgres::PostgresReminderRepository> postgres_reminders;

    if (postgres_enabled())
    {
      database.emplace(
          virtual_planner::infrastructure::postgres::PostgresConfig::from_environment());
      database->connect();
      std::cout << "PostgreSQL connection established\n";

      postgres_goals.emplace(*database);
      postgres_reminders.emplace(*database);

      repositories.goals = &*postgres_goals;
      repositories.reminders = &*postgres_reminders;
      health_database = &*database;
    }
#else
    if (postgres_enabled())
    {
      std::cerr << "PostgreSQL support was not compiled. Rebuild with "
                << "-DVIRTUAL_PLANNER_WITH_POSTGRES=ON\n";
      return 1;
    }
#endif

#if defined(VIRTUAL_PLANNER_WITH_HTTP)
    const auto server_config =
        virtual_planner::api::http::ServerConfig::from_environment();

    virtual_planner::api::http::ApiServer server(config, repositories, health_database);

    const int port = server.bind(server_config);

    if (port < 0)
    {
      std::cerr << "Failed to bind " << server_config.host << ':'
                << server_config.port << ".\n";
      return 1;
    }

    std::cout << "Serving http://" << server_config.host << ':' << port
              << "/api/health\n";

    if (!server.serve())
    {
      std::cerr << "HTTP server stopped with an error.\n";
      return 1;
    }
#else
    std::cout << "HTTP server was not compiled. Rebuild with "
              << "-DVIRTUAL_PLANNER_WITH_HTTP=ON to serve /api/health\n";
#endif

    return 0;
  }
  catch (const virtual_planner::shared::ApplicationError &error)
  {
    // As mensagens de PostgresConfig ja vem com a senha mascarada.
    std::cerr << "Startup failed: " << error.what() << '\n';
    return 1;
  }
}
