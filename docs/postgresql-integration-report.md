# Relatório Da Integração PostgreSQL

## Estado Anterior

O projeto tinha suporte arquitetural para persistência por meio de `persistence::Database`, mas não possuía integração real com PostgreSQL.

Não existiam:

- Driver `libpqxx` ou `libpq` configurado.
- String de conexão.
- Adapter concreto.
- Transações reais.
- Testes de integração PostgreSQL.
- Docker local para banco.

## Decisão Arquitetural

PostgreSQL foi implementado como adapter opcional em `infrastructure/postgres`. A abstração `Database` continua sem dependência de PostgreSQL.

## Dependência Escolhida

`libpqxx`, por oferecer API C++ idiomática, RAII e transações convenientes.

## Arquivos Criados

- `include/virtual_planner/persistence/transaction.hpp`
- `include/virtual_planner/infrastructure/postgres/postgres_config.hpp`
- `include/virtual_planner/infrastructure/postgres/postgres_database.hpp`
- `include/virtual_planner/infrastructure/postgres/postgres_transaction.hpp`
- `src/infrastructure/postgres/postgres_config.cpp`
- `src/infrastructure/postgres/postgres_database.cpp`
- `src/infrastructure/postgres/postgres_transaction.cpp`
- `tests/unit/infrastructure/postgres/postgres_config_test.cpp`
- `tests/integration/postgres/postgres_integration_test.cpp`
- `.env.example`
- `docker-compose.yml`
- `migrations/README.md`
- `docs/postgresql.md`
- `docs/persistence-architecture.md`
- `docs/adr/ADR-001-postgresql-adapter.md`

## Arquivos Modificados

- `CMakeLists.txt`
- `src/main.cpp`
- `.gitignore`
- `README.md`
- `docs/architecture.md`
- `docs/conventions.md`
- `docs/getting-started.md`

## Configuração

Variáveis PostgreSQL:

- `POSTGRES_HOST`
- `POSTGRES_PORT`
- `POSTGRES_DB`
- `POSTGRES_USER`
- `POSTGRES_PASSWORD`
- `POSTGRES_SSLMODE`
- `POSTGRES_CONNECT_TIMEOUT`
- `POSTGRES_APPLICATION_NAME`

Habilitação em runtime:

- `VP_USE_POSTGRES=true`

Habilitação em build:

- `VIRTUAL_PLANNER_WITH_POSTGRES=ON`

## Testes

Testes unitários adicionados:

- `postgres_config_test`

Teste de integração adicionado:

- `postgres_integration_test`

## Comandos Validados

```bash
cmake -S . -B build-opencode
cmake --build build-opencode
ctest --test-dir build-opencode --output-on-failure
```

Resultado: build padrão e 3 testes passaram.

## Comandos Não Validados Neste Ambiente

```bash
cmake -S . -B build-opencode-postgres -DVIRTUAL_PLANNER_WITH_POSTGRES=ON
```

Resultado: falhou corretamente porque o ambiente não possui `libpqxx` nem `pkg-config` disponível para localização da dependência.

## Documentação Atualizada

- `README.md`
- `docs/architecture.md`
- `docs/conventions.md`
- `docs/getting-started.md`
- `docs/postgresql.md`
- `docs/persistence-architecture.md`
- `docs/adr/ADR-001-postgresql-adapter.md`

## Pendências

- Instalar `libpqxx` no ambiente local ou CI.
- Validar build com `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- Subir PostgreSQL local via Docker e executar `postgres_integration_test`.
- Criar schema e migrations apenas quando houver entidades reais.
- Criar repositórios concretos apenas quando houver casos de uso reais.
