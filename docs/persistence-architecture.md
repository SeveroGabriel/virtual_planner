# Arquitetura De Persistência

## Objetivo

Permitir persistência real com PostgreSQL sem acoplar o núcleo da aplicação ao fornecedor de banco de dados.

## Componentes

- `persistence::Database`: controla ciclo de vida genérico de persistência.
- `persistence::Transaction`: contrato mínimo de transação.
- `infrastructure::postgres::PostgresConfig`: valida e monta configuração PostgreSQL.
- `infrastructure::postgres::PostgresDatabase`: abre, valida e encerra conexão PostgreSQL via `libpqxx`.
- `infrastructure::postgres::PostgresTransaction`: encapsula `pqxx::work` e garante rollback automático se a transação sair de escopo sem `commit()`.

## Fluxo De Inicialização

1. `main` carrega configuração geral com `EnvironmentConfigLoader`.
2. Se `VP_USE_POSTGRES=true` e o binário foi compilado com `VIRTUAL_PLANNER_WITH_POSTGRES=ON`, `main` cria `PostgresConfig::from_environment()`.
3. `PostgresDatabase::connect()` chama `initialize()` quando necessário.
4. `PostgresDatabase::on_initialize()` valida a configuração.
5. `PostgresDatabase::on_connect()` abre a conexão `libpqxx` e executa `SELECT 1`.
6. `shutdown()` fecha e libera a conexão.

## Regra De Dependência

```text
domain/application
      |
      v
interfaces + persistence
      ^
      |
infrastructure/postgres
      |
      v
libpqxx
```

`libpqxx` não aparece no domínio, aplicação, core ou abstrações base de persistência.

## Transações

`PostgresTransaction` deve ser usado quando uma operação exigir atomicidade. O comportamento atual é:

- `commit()` confirma a transação.
- `rollback()` aborta explicitamente.
- O destrutor aborta automaticamente se a transação ainda estiver ativa.

## Repositórios

Ainda não há repositórios concretos PostgreSQL porque o projeto não possui entidades de domínio. Quando surgirem, as implementações devem ficar em `infrastructure/postgres` e usar queries parametrizadas.

## Limitações

- Sem pool de conexões.
- Sem migrations aplicadas automaticamente.
- Sem repositórios concretos.
- Sem thread-safety documentada para uso concorrente.

Essas limitações são intencionais para evitar complexidade prematura no escopo acadêmico atual.
