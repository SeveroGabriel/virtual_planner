# Arquitetura De Persistência

## Objetivo

Permitir persistência real com PostgreSQL sem acoplar o núcleo da aplicação ao fornecedor de banco de dados.

## Componentes

- `persistence::Database`: controla ciclo de vida genérico de persistência.
- `persistence::Transaction`: contrato mínimo de transação.
- `persistence::*Repository`: portas de persistência para entidades de domínio.
- `infrastructure::postgres::PostgresConfig`: valida e monta configuração PostgreSQL.
- `infrastructure::postgres::PostgresDatabase`: abre, valida e encerra conexão PostgreSQL via `libpqxx`.
- `infrastructure::postgres::PostgresTransaction`: encapsula `pqxx::work` e garante rollback automático se a transação sair de escopo sem `commit()`.
- `persistence::InMemoryGoalRepository`, `InMemoryTaskRepository`, `InMemoryReminderRepository`, `InMemoryUserRepository`: implementações concretas em memória dos contratos de repositório, em `persistence/memory/`. Servem aos testes e a um modo de execução sem banco.

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

O projeto já possui contratos de repositório para entidades do domínio:

- `UserRepository`.
- `TaskRepository`.
- `GoalRepository`.
- `ReminderRepository`.

Esses contratos ficam em `persistence` porque são portas estáveis do núcleo. Hoje existe uma implementação concreta de cada um em memória, em `persistence/memory/` (`InMemoryGoalRepository`, `InMemoryTaskRepository`, `InMemoryReminderRepository`, `InMemoryUserRepository`), usada pelos testes e por um modo de execução sem banco. Ainda não há implementação concreta PostgreSQL para esses repositórios. Quando forem criadas (issues #26.1 a #26.4), as implementações devem ficar em `infrastructure/postgres` e usar queries parametrizadas.

## Semântica de save

As quatro implementações em memória fixam uma semântica de referência para `save` que qualquer implementação futura — em particular a PostgreSQL — precisa reproduzir:

1. **`Goal::save` gera o id e ignora `goal.id()`; os outros três preservam o id da entidade.** Consequência prática: "criar com um id escolhido" é possível para três entidades e impossível para Goal; e "alterar um registro existente" é `update()` no Goal e `save()` nos outros três. Verbos opostos para a mesma intenção.
2. **Os três `save` sem `update` são upsert** — substituem quem já tem o mesmo `id()`, inserem caso contrário. Isso existe porque os contratos congelados não deram `update` a `TaskRepository`, `ReminderRepository` nem `UserRepository`. Consequência para quem for implementar os repositórios PostgreSQL (issues #26.1 a #26.4): precisam de `INSERT ... ON CONFLICT (id) DO UPDATE`, não `INSERT` simples, e o id vem do chamador, não de uma sequence. Um `INSERT` simples passa em todos os testes in-memory e diverge em runtime.
3. **A ordem de `find_all()` não é especificada.** O in-memory devolve ordem de inserção; o PostgreSQL sem `ORDER BY` devolve o que o planner der. Nenhum teste fixa isso hoje, então um teste de caso de uso que assere `find_all()[0]` passaria in-memory e quebraria no PostgreSQL. Quem precisar de ordem estável deve ordenar explicitamente no caso de uso.

## Limitações

- Sem pool de conexões.
- Sem migrations aplicadas automaticamente.
- Sem schema SQL real.
- Sem repositórios concretos PostgreSQL para entidades de domínio.
- Os repositórios em memória (`persistence/memory/`) não são thread-safe: cada um guarda um `std::vector` sem lock nenhum. O `httplib::Server` escolhido na ADR-003 despacha handlers num thread pool por padrão, então quando a Onda 2 (issue #29 / P-28) ligar um repositório em memória atrás desse servidor, chamadas concorrentes vão mutar o `std::vector` sem proteção — uma data race que se manifesta como corrupção intermitente, não como falha limpa. Quem serializar o acesso é o chamador; nada nesses repositórios faz isso hoje.

Essas limitações são intencionais para evitar complexidade prematura no escopo acadêmico atual.
