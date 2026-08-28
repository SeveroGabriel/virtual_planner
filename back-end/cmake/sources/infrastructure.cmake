# Fontes de infraestrutura que nao dependem de PostgreSQL.
# O adapter PostgreSQL fica em cmake/sources/postgres.cmake, incluido apenas
# quando VIRTUAL_PLANNER_WITH_POSTGRES=ON.
virtual_planner_add_sources(
  infrastructure/config/environment_config_loader.cpp
  infrastructure/logging/console_logger.cpp
  infrastructure/postgres/postgres_config.cpp
)
