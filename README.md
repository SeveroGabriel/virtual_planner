# Virtual Planner

Virtual Planner é um projeto acadêmico em C++20 que fornece uma fundação modular simples para um backend. A base atual define estrutura, contratos, configuração, ciclo de vida de persistência, adapter opcional para PostgreSQL e testes automatizados, mas ainda não implementa regras específicas de produto ou de domínio.

A proposta é manter a arquitetura clara sem antecipar complexidade: o núcleo continua independente de fornecedor de banco, e detalhes concretos ficam em `infrastructure`.

## Objetivos

- Manter o código simples o suficiente para um projeto de faculdade.
- Separar domínio, aplicação, infraestrutura, persistência, interfaces e código compartilhado.
- Compilar o projeto com CMake usando C++20.
- Manter o núcleo vendor-neutral, sem dependência direta de PostgreSQL.
- Usar PostgreSQL por meio de adapter concreto opcional em `infrastructure/postgres`.
- Manter testes rápidos, locais e independentes de banco real por padrão.

## Requisitos

- Compilador com suporte a C++20.
- CMake 3.20 ou mais recente.
- `libpqxx` somente quando `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- Docker opcional para subir PostgreSQL local.

O projeto foi validado no macOS com Apple clang e CMake 4.4.0.

## Build Sem PostgreSQL

Este é o caminho padrão. Ele compila o núcleo, a aplicação e os testes unitários sem exigir driver PostgreSQL instalado.

```bash
cmake -S . -B build
cmake --build build
```

## Build Com PostgreSQL

Para compilar o adapter concreto `PostgresDatabase`, instale `libpqxx` e gere o build com a flag:

```bash
cmake -S . -B build-postgres -DVIRTUAL_PLANNER_WITH_POSTGRES=ON
cmake --build build-postgres
```

O CMake tenta localizar `libpqxx` por pacote CMake e, se não encontrar, por `pkg-config`. Se a dependência estiver ausente, a configuração falha com mensagem explícita.

## Execução

Sem PostgreSQL:

```bash
./build/virtual_planner
```

Com PostgreSQL compilado e habilitado em runtime:

```bash
VP_USE_POSTGRES=true ./build-postgres/virtual_planner
```

Se `VP_USE_POSTGRES=true` for usado em um build sem `VIRTUAL_PLANNER_WITH_POSTGRES=ON`, a aplicação encerra com erro informando que o suporte PostgreSQL não foi compilado.

## Configuração PostgreSQL

O adapter lê configuração externa por variáveis de ambiente:

- `POSTGRES_HOST`, padrão `localhost`.
- `POSTGRES_PORT`, padrão `5432`.
- `POSTGRES_DB`, obrigatório.
- `POSTGRES_USER`, obrigatório.
- `POSTGRES_PASSWORD`, obrigatório.
- `POSTGRES_SSLMODE`, padrão `disable`.
- `POSTGRES_CONNECT_TIMEOUT`, padrão `5`.
- `POSTGRES_APPLICATION_NAME`, padrão `virtual-planner`.

Use `.env.example` como referência. O arquivo `.env` real é ignorado pelo Git e não deve conter segredos versionados.

## Docker

Para subir PostgreSQL local:

```bash
docker compose up -d postgres
docker compose ps
```

Comandos úteis:

```bash
docker compose logs -f postgres
docker compose stop
docker compose down
```

Use `docker compose down -v` apenas quando quiser apagar também o volume de dados local.

## Testes

Testes unitários padrão:

```bash
ctest --test-dir build --output-on-failure
```

Testes atuais no build padrão:

- `app_config_test`
- `database_test`
- `postgres_config_test`

Teste de integração PostgreSQL, disponível apenas no build com `VIRTUAL_PLANNER_WITH_POSTGRES=ON`:

```bash
ctest --test-dir build-postgres --output-on-failure -R postgres_integration_test
```

O teste de integração usa `POSTGRES_DB`, `POSTGRES_USER` e `POSTGRES_PASSWORD`. Se essas variáveis não estiverem definidas, ele é pulado com mensagem clara.

## Estrutura do Projeto

```text
.
├── include/virtual_planner
│   ├── application
│   ├── core
│   ├── domain
│   ├── infrastructure
│   │   ├── config
│   │   └── postgres
│   ├── interfaces
│   ├── persistence
│   └── shared
├── src
│   ├── application
│   ├── core
│   ├── domain
│   ├── infrastructure
│   │   ├── config
│   │   └── postgres
│   ├── persistence
│   ├── shared
│   └── main.cpp
├── tests
│   ├── integration
│   └── unit
├── docs
├── migrations
├── docker-compose.yml
└── CMakeLists.txt
```

## Arquitetura De Persistência

- `persistence::Database`: abstração vendor-neutral para ciclo de vida de persistência.
- `persistence::Transaction`: contrato mínimo para `commit()` e `rollback()`.
- `infrastructure::postgres::PostgresConfig`: configuração externa e segura para conexão PostgreSQL.
- `infrastructure::postgres::PostgresDatabase`: adapter concreto baseado em `libpqxx`, compilado apenas com `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- `infrastructure::postgres::PostgresTransaction`: transação PostgreSQL com rollback automático no destrutor se não houver `commit()`.

Regra principal: dependências devem apontar para dentro. `domain`, `application`, `core` e `persistence::Database` não incluem headers de PostgreSQL. O driver aparece apenas na infraestrutura concreta.

## Migrações

O diretório `migrations/` está reservado para SQL versionado. Ainda não há schema inicial porque o projeto não possui entidades de domínio definidas.

## Documentação

- `docs/architecture.md`: objetivo arquitetural, camadas, regra de dependência, persistência, configuração e dependências externas.
- `docs/conventions.md`: convenções de linguagem, nomenclatura, arquivos, includes, testes e arquitetura.
- `docs/getting-started.md`: comandos para compilar, testar e executar o projeto.
- `docs/persistence-architecture.md`: detalhes da arquitetura de persistência.
- `docs/postgresql.md`: guia de configuração, build, Docker, testes, segurança e troubleshooting do PostgreSQL.
- `docs/adr/ADR-001-postgresql-adapter.md`: decisão arquitetural do adapter PostgreSQL.
- `docs/postgresql-integration-report.md`: relatório da integração implementada.

## Status Atual

- Build com CMake sem PostgreSQL: funcionando.
- Testes unitários sem PostgreSQL: funcionando.
- Adapter PostgreSQL: implementado como feature opcional.
- Transações PostgreSQL: implementadas via `PostgresTransaction`.
- Banco de dados real: disponível apenas quando `libpqxx` estiver instalado e a flag estiver habilitada.
- Regras de negócio: ainda não implementadas.
- Repositórios concretos de domínio: ainda não implementados porque não há entidades reais.
- Schema/migrations reais: ainda não implementados porque não há modelo de domínio definido.

## Próximos Passos

1. Instalar `libpqxx` no ambiente de desenvolvimento ou CI para validar o build com `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
2. Executar o teste de integração com PostgreSQL local via Docker.
3. Implementar o primeiro caso de uso em `application` quando a regra do trabalho estiver definida.
4. Adicionar entidades em `domain` apenas quando houver regras de negócio reais.
5. Criar repositórios PostgreSQL específicos para entidades reais, usando queries parametrizadas.
6. Adicionar scripts SQL versionados em `migrations/` quando houver schema real.
