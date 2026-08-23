# Virtual Planner

Virtual Planner é um projeto acadêmico desenvolvido em C++20 para ajudar no planejamento pessoal.

O projeto possui uma estrutura inicial para usuários, tarefas, metas e lembretes. Também oferece suporte opcional ao PostgreSQL.

## Objetivos

- Manter o código simples e organizado.
- Separar as principais partes do sistema.
- Compilar o projeto com CMake e C++20.
- Permitir o uso opcional do PostgreSQL.
- Criar uma base para tarefas, metas e lembretes.
- Manter testes que não dependam de um banco real.

## Tecnologias

- **C++20**: linguagem do backend.
- **CMake 3.20+**: build system, modularizado em `back-end/cmake/`.
- **CTest**: execução da suíte de testes, sem framework externo.
- **PostgreSQL**: banco de dados, usado através de um adapter opcional.
- **libpqxx**: cliente C++ do PostgreSQL, exigido apenas quando `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- **Docker Compose**: PostgreSQL local para desenvolvimento e testes de integração.
- **GitHub Actions**: CI do backend, em `.github/workflows/backend.yml`.

## Requisitos

- Compilador com suporte a C++20.
- CMake 3.20 ou superior.
- `libpqxx` apenas para usar PostgreSQL.
- Docker opcional.

## Compilação

### Sem PostgreSQL

Esta é a opção padrão:

```bash
cmake -S back-end -B back-end/build
cmake --build back-end/build
```

### Com PostgreSQL

É necessário instalar o `libpqxx` antes de compilar:

```bash
cmake -S back-end -B back-end/build-postgres -DVIRTUAL_PLANNER_WITH_POSTGRES=ON
cmake --build back-end/build-postgres
```

No macOS com Homebrew, o `libpq` é keg-only e fica fora do prefixo padrão que o CMake procura. Como o `libpqxx` depende dele, é preciso informar os dois prefixos:

```bash
cmake -S back-end -B back-end/build-postgres -DVIRTUAL_PLANNER_WITH_POSTGRES=ON \
  -DCMAKE_PREFIX_PATH="$(brew --prefix libpqxx);$(brew --prefix libpq)"
```

## Execução

Sem PostgreSQL:

```bash
./back-end/build/virtual_planner
```

Com PostgreSQL:

```bash
VP_USE_POSTGRES=true ./back-end/build-postgres/virtual_planner
```

## Configuração do PostgreSQL

As configurações são feitas por variáveis de ambiente:

- `POSTGRES_HOST`: padrão `localhost`.
- `POSTGRES_PORT`: padrão `5432`.
- `POSTGRES_DB`: nome do banco.
- `POSTGRES_USER`: usuário.
- `POSTGRES_PASSWORD`: senha.
- `POSTGRES_SSLMODE`: padrão `disable`.

Use o arquivo `.env.example` como referência. Não envie o arquivo `.env` para o Git.

## Docker

Para iniciar o PostgreSQL localmente:

```bash
docker compose up -d postgres
```

Comandos úteis:

```bash
docker compose ps
docker compose logs -f postgres
docker compose stop
docker compose down
```

Use `docker compose down -v` somente para apagar também os dados locais.

## Migrações Do Banco De Dados

Com o PostgreSQL de pé (`docker compose up -d postgres`), aplique as migrações de `back-end/migrations/` com:

```bash
./scripts/db-migrate.sh
```

O script é idempotente (não reaplica migrações já registradas), roda cada migração em transação e usa as mesmas variáveis de ambiente de `back-end/.env.example` (`POSTGRES_HOST`, `POSTGRES_PORT`, `POSTGRES_DB`, `POSTGRES_USER`, `POSTGRES_PASSWORD`, `POSTGRES_SSLMODE`). Veja `back-end/migrations/README.md` para detalhes.

## Testes

Testes padrão:

```bash
ctest --test-dir back-end/build --output-on-failure
```

Teste de integração com PostgreSQL:

```bash
ctest --test-dir back-end/build-postgres --output-on-failure -R postgres_integration_test
```

O teste de integração precisa das variáveis `POSTGRES_DB`, `POSTGRES_USER` e `POSTGRES_PASSWORD`, e do schema aplicado via `./scripts/db-migrate.sh`.

## Estrutura do Projeto

```text
.
├── back-end
│   ├── include/virtual_planner
│   │   ├── application
│   │   ├── core
│   │   ├── domain
│   │   │   ├── entities
│   │   │   ├── enums
│   │   │   └── value_objects
│   │   ├── infrastructure
│   │   │   ├── config
│   │   │   └── postgres
│   │   ├── interfaces
│   │   ├── persistence
│   │   └── shared
│   ├── src
│   ├── tests
│   ├── migrations
│   ├── .env.example
│   └── CMakeLists.txt
├── scripts
│   └── db-migrate.sh
├── docs
├── .github/workflows
├── docker-compose.yml
└── README.md
```

## Arquitetura

O projeto está dividido em camadas:

- `domain`: entidades e regras do sistema.
- `application`: casos de uso do sistema. O módulo Goal já está implementado (criar, atualizar, remover, listar e alterar status de metas).
- `interfaces`: contratos usados pelas diferentes partes do projeto.
- `persistence`: contratos para banco de dados e repositórios.
- `infrastructure`: implementações externas, como configuração e PostgreSQL.
- `core` e `shared`: configurações, erros e recursos compartilhados.
- `main.cpp`: inicia e configura a aplicação.

O código principal não depende diretamente do PostgreSQL. A implementação do banco fica em `infrastructure/postgres`.

![Diagrama da arquitetura atual do Virtual Planner](docs/diagrams/current-architecture.webp)

Outros arquivos do diagrama:

- [`docs/diagrams/current-architecture.html`](docs/diagrams/current-architecture.html)
- [`docs/diagrams/current-architecture.architecture.json`](docs/diagrams/current-architecture.architecture.json)

### Contratos De Persistência

- `persistence::Database`: abstração de ciclo de vida de persistência, independente de fornecedor.
- `persistence::Transaction`: contrato mínimo para `commit()` e `rollback()`.
- `persistence::*Repository`: contratos de repositório para as entidades de domínio. `GoalRepository` já possui implementação concreta em PostgreSQL; `TaskRepository`, `ReminderRepository` e `UserRepository` ainda são apenas contratos.
- `infrastructure::postgres::PostgresConfig`: configuração externa da conexão PostgreSQL.
- `infrastructure::postgres::PostgresDatabase`: adapter concreto baseado em `libpqxx`, compilado apenas com `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- `infrastructure::postgres::PostgresTransaction`: transação PostgreSQL com rollback automático no destrutor se não houver `commit()`.
- `infrastructure::postgres::PostgresGoalRepository`: implementação concreta de `persistence::GoalRepository` sobre `libpqxx`.

## Domínio Inicial

O projeto possui as seguintes entidades:

- `User`
- `Task`
- `Goal`
- `Reminder`

Também possui tipos auxiliares para datas, horários, categorias, prioridades e status:

- Value objects: `Date` e `TimeSlot`.
- Enums: `Category`, `Priority`, `TaskStatus`, `GoalStatus`, `GoalPeriod`, `ReminderType`, `ReminderRecurrence` e `Shift`.

## Documentação

Documentos adicionais estão disponíveis na pasta `docs/`:

- [`docs/getting-started.md`](docs/getting-started.md): primeiros passos.
- [`docs/architecture.md`](docs/architecture.md): decisões de arquitetura.
- [`docs/conventions.md`](docs/conventions.md): convenções de código, testes e build.
- [`docs/persistence-architecture.md`](docs/persistence-architecture.md): camada de persistência.
- [`docs/postgresql.md`](docs/postgresql.md): uso do PostgreSQL.
- [`docs/date-timeslot-contract.md`](docs/date-timeslot-contract.md): contrato público congelado de `Date` e `TimeSlot`.
- [`back-end/migrations/README.md`](back-end/migrations/README.md): convenção de numeração das migrações.

O planejamento e o estado das tarefas ficam nas issues do GitHub, não neste arquivo.
