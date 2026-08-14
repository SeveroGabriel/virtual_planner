# Testes de infraestrutura que nao exigem um banco de dados real.
virtual_planner_add_test(
  postgres_config_test
  unit/infrastructure/postgres/postgres_config_test.cpp
)
