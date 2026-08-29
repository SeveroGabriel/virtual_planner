# Virtual Planner

Virtual Planner é um projeto acadêmico desenvolvido em C++20 para ajudar no planejamento pessoal.

O domínio cobre usuários, tarefas, metas e lembretes. Sobre ele já existem casos de uso de Goal e Reminder, um serviço de relatórios, uma API HTTP que serve o CRUD de metas mais relatórios e dashboard, e adapters PostgreSQL para Goal e Reminder. O front-end em React consome mocks e ainda não chama a API.

O build padrão continua sem rede e sem banco: HTTP, JSON, PostgreSQL e cobertura são opções desligadas por padrão.

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
- **cpp-httplib** e **nlohmann/json**: servidor HTTP e serialização, baixados por `FetchContent` apenas com as flags de build ligadas.
- **React 19 + TypeScript + Vite + Tailwind CSS v4**: front-end, em `front-end/`.
- **PostgreSQL**: banco de dados, usado através de um adapter opcional.
- **libpqxx**: cliente C++ do PostgreSQL, exigido apenas quando `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- **Docker Compose**: PostgreSQL local para desenvolvimento e testes de integração.
- **GitHub Actions**: CI do backend em `.github/workflows/backend.yml` (build padrão, JSON/HTTP, PostgreSQL e cobertura) e do front-end em `.github/workflows/frontend.yml`.

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

### Com a API HTTP

O servidor e a serialização JSON dependem de bibliotecas baixadas por `FetchContent`, então ficam atrás de uma opção desligada por padrão — o build padrão nunca toca a rede:

```bash
cmake -S back-end -B back-end/build-http -DVIRTUAL_PLANNER_WITH_HTTP=ON
cmake --build back-end/build-http
```

Para compilar apenas a serialização compartilhada, sem o servidor, use `-DVIRTUAL_PLANNER_WITH_JSON=ON`.

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

### Com cobertura de testes

```bash
cmake -S back-end -B back-end/build-coverage -DVIRTUAL_PLANNER_WITH_COVERAGE=ON
cmake --build back-end/build-coverage
ctest --test-dir back-end/build-coverage --output-on-failure
```

O job `Cobertura de testes` do CI publica o percentual no resumo da execução e o relatório HTML como artefato.

## Execução

Sem PostgreSQL:

```bash
./back-end/build/virtual_planner
```

Com PostgreSQL:

```bash
VP_USE_POSTGRES=true ./back-end/build-postgres/virtual_planner
```

No build padrão, sem a opção `VIRTUAL_PLANNER_WITH_HTTP`, o executável imprime a configuração e encerra. Com a API compilada, ele sobe o servidor:

```bash
VP_HTTP_HOST=127.0.0.1 VP_HTTP_PORT=8080 ./back-end/build-http/virtual_planner
curl -s http://127.0.0.1:8080/api/health
```

`VP_HTTP_HOST` e `VP_HTTP_PORT` são opcionais e caem em `0.0.0.0:8080` — dentro de container é o que permite ao Docker publicar a porta; fora dele, prefira `127.0.0.1`. A API sobe e responde mesmo sem PostgreSQL.

Endpoints disponíveis hoje:

| Método e rota | O que faz |
| --- | --- |
| `GET /api/health` | responde sempre 200, e informa se o banco está configurado e conectado |
| `GET /api/goals?period=&date=` | lista metas do período civil (`weekly`, `monthly` ou `yearly`) |
| `GET /api/goals/:id` | busca uma meta |
| `POST /api/goals` | cria uma meta |
| `PATCH /api/goals/:id` | atualização parcial |
| `PATCH /api/goals/:id/status` | altera só o status |
| `DELETE /api/goals/:id` | remove uma meta |
| `GET /api/reports?period=&date=` | métricas do período civil, mesmos valores de `period` |
| `GET /api/dashboard` | resumo do dia |

Erro de domínio vira status HTTP num mapeamento único: `400` para validação, `404` para não encontrado, `409` para conflito e `500` genérico, sem vazar a mensagem interna. O contrato completo, com CORS e log, está em [docs/api.md](docs/api.md).

Os endpoints de Task, Reminder e User ainda não existem.

> **A API não tem autenticação.** Qualquer pessoa que alcance a porta lê, altera e
> apaga dados, e lê todos os relatórios. É uma decisão registrada na ADR-002, que
> descreve o sistema como single-tenant de uso local — e vale enquanto o servidor
> ficar no `localhost`. Por isso `VP_HTTP_HOST` deve continuar em `127.0.0.1` fora
> de container, e o `docker-compose.yml` publica todas as portas apenas no
> loopback. Não mude para `0.0.0.0` antes de existir autenticação.

## Configuração do PostgreSQL

As configurações são feitas por variáveis de ambiente:

- `POSTGRES_HOST`: padrão `localhost`.
- `POSTGRES_PORT`: padrão `5432`.
- `POSTGRES_DB`: nome do banco.
- `POSTGRES_USER`: usuário.
- `POSTGRES_PASSWORD`: senha.
- `POSTGRES_SSLMODE`: padrão `disable`.

## Variáveis De Ambiente

Existe um `.env.example` por workspace, cada um com um escopo:

| Arquivo | Alimenta | Contém |
| --- | --- | --- |
| `.env.example` (raiz) | `docker-compose.yml` | `POSTGRES_DB`, `POSTGRES_USER`, `POSTGRES_PASSWORD` — os três valores que **criam** o banco no container |
| `back-end/.env.example` | o executável do backend e `scripts/db-migrate.sh` | `VP_*` (nome, perfil, host e porta HTTP) e `POSTGRES_*` — em qual banco **conectar** |
| `front-end/.env.example` | o build do Vite | nada por enquanto: o frontend ainda não lê variável nenhuma. O arquivo fixa as regras do prefixo `VITE_` |

`POSTGRES_DB`, `POSTGRES_USER` e `POSTGRES_PASSWORD` aparecem em dois arquivos de propósito — criar o banco e conectar nele são coisas diferentes. Se mudar de um lado, mude do outro.

Copie o `.env.example` de cada workspace para `.env` no mesmo diretório e ajuste os valores. Nenhum `.env` vai para o Git: o `.gitignore` ignora `.env` e `.env.*`, com exceção explícita para `.env.example`.

Tudo com o prefixo `VITE_` é embutido no bundle e fica visível no navegador. Nunca coloque senha ou token em `front-end/.env`.

## Docker

### Stack completa

`POSTGRES_PASSWORD` é obrigatória e não tem valor padrão: sem ela o compose para
com erro em vez de subir com uma senha publicada neste repositório. De um clone
limpo:

```bash
cp .env.example .env    # e troque POSTGRES_PASSWORD
docker compose up
```

| Serviço | Porta | O que é |
| --- | --- | --- |
| `postgres` | 5432 | PostgreSQL 16 |
| `migrate` | — | roda `scripts/db-migrate.sh` uma vez e sai |
| `api` | 8080 | backend com HTTP e PostgreSQL compilados |
| `web` | 8081 | build de produção do frontend servido por nginx |

Depois de subir:

```bash
curl -s http://127.0.0.1:8080/api/health   # {"status":"ok", ...}
open http://127.0.0.1:8081                 # frontend
```

A ordem é garantida por `depends_on` com condição: a API só sobe depois que o banco está saudável **e** as migrações terminaram, e o frontend só depois que a API responde `/api/health`. O `migrate` é idempotente, então repetir `docker compose up` não reaplica nada.

O healthcheck da API confere o campo `status` da resposta, não só o código HTTP — `/api/health` responde 200 mesmo com o banco fora do ar, então checar só o status HTTP não provaria integração.

**Primeiro build demora** (alguns minutos): a imagem do backend parte do `ubuntu:24.04` e compila o `libpqxx` 8.x a partir do código-fonte, porque a distribuição empacota a 7.x e o adapter usa a API 8.x. Os builds seguintes reaproveitam a camada.

### Só o banco

Para desenvolver com o backend rodando na máquina:

```bash
docker compose up -d postgres
```

### Comandos úteis

```bash
docker compose ps
docker compose logs -f api
docker compose stop
docker compose down
```

Use `docker compose down -v` somente para apagar também os dados locais.

Todas as portas são publicadas em `127.0.0.1`, e não em `0.0.0.0`: nem o banco nem
a API ficam alcançáveis de outra máquina. Os serviços conversam entre si pela rede
interna do compose, pelo nome do serviço, então nada disso depende da publicação.

### Credenciais

Nenhuma credencial vai para dentro das imagens. Tudo entra por variável de ambiente do compose.

`POSTGRES_PASSWORD` não tem valor padrão em lugar nenhum — nem no compose, nem no `.env.example`, nem no `scripts/db-migrate.sh`. O placeholder do `.env.example` é `DEFINA-UMA-SENHA`, escolhido justamente por **não** funcionar: um placeholder que conecta é pior que nenhum, porque quem esquece de trocá-lo não descobre.

Com `VP_PROFILE=production` a aplicação recusa subir se a senha for um valor conhecido (`change-me`, `postgres`, `password`...) ou se `POSTGRES_SSLMODE` for `disable`.

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
│   │   ├── api
│   │   │   ├── http
│   │   │   │   └── routes        # uma unidade por grupo de endpoints
│   │   │   └── json
│   │   ├── application
│   │   │   ├── goal
│   │   │   ├── reminder
│   │   │   └── reporting
│   │   ├── core
│   │   ├── domain
│   │   │   ├── entities
│   │   │   ├── enums
│   │   │   └── value_objects
│   │   ├── infrastructure
│   │   │   ├── config
│   │   │   ├── logging
│   │   │   └── postgres
│   │   ├── interfaces
│   │   ├── persistence
│   │   │   └── memory           # repositorios in-memory
│   │   └── shared
│   ├── src
│   ├── tests                    # unit/ e integration/
│   ├── cmake                    # sources/ e tests/, um arquivo por modulo
│   ├── migrations
│   ├── Dockerfile
│   ├── .env.example
│   └── CMakeLists.txt
├── front-end
│   ├── src
│   ├── Dockerfile
│   ├── nginx.conf
│   └── package.json
├── scripts
│   └── db-migrate.sh
├── docs
│   └── diagrams
├── .github/workflows
├── docker-compose.yml
└── README.md
```

## Arquitetura

O projeto está dividido em camadas:

- `domain`: entidades e regras do sistema.
- `application`: casos de uso e serviços. Goal está completo (criar, buscar, atualizar, remover, listar e alterar status), Reminder tem criação, atualização, remoção e listagem com recorrência, e `reporting` calcula as métricas do contrato da P-63. Task ainda não tem casos de uso.
- `api`: fronteira HTTP e serialização JSON. Depende das camadas internas, mas nenhuma delas depende de `api` — `httplib` e `nlohmann` só aparecem aqui.
- `interfaces`: contratos usados pelas diferentes partes do projeto.
- `persistence`: contratos para banco de dados e repositórios.
- `infrastructure`: implementações externas, como configuração e PostgreSQL.
- `core` e `shared`: configurações, erros e recursos compartilhados.
- `infrastructure/logging`: `ConsoleLogger`, adapter da porta `Logger`.
- `main.cpp`: inicia e configura a aplicação.

O código principal não depende diretamente do PostgreSQL. A implementação do banco fica em `infrastructure/postgres`.

![Diagrama da arquitetura atual do Virtual Planner](docs/diagrams/current-architecture.webp)

Outros arquivos do diagrama:

- [`docs/diagrams/current-architecture.html`](docs/diagrams/current-architecture.html)
- [`docs/diagrams/current-architecture.architecture.json`](docs/diagrams/current-architecture.architecture.json)

### Contratos De Persistência

- `persistence::Database`: abstração de ciclo de vida de persistência, independente de fornecedor.
- `persistence::Transaction`: contrato mínimo para `commit()` e `rollback()`.
- `persistence::*Repository`: contratos de repositório para as entidades de domínio. Todos têm implementação in-memory em `persistence/memory`. `GoalRepository` e `ReminderRepository` já possuem adapter PostgreSQL; `TaskRepository` e `UserRepository` seguem só com in-memory.
- `infrastructure::postgres::PostgresConfig`: configuração externa da conexão PostgreSQL.
- `infrastructure::postgres::PostgresDatabase`: adapter concreto baseado em `libpqxx`, compilado apenas com `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- `infrastructure::postgres::PostgresTransaction`: transação PostgreSQL com rollback automático no destrutor se não houver `commit()`.
- `infrastructure::postgres::PostgresGoalRepository` e `PostgresReminderRepository`: implementações concretas sobre `libpqxx`.

## Domínio Inicial

O projeto possui as seguintes entidades:

- `User`
- `Task`
- `Goal`
- `Reminder`

Também possui tipos auxiliares para datas, horários, categorias, prioridades e status:

- Value objects: `Date` e `TimeSlot`, com contrato público congelado pela P-61.
- Enums: `Category`, `Priority`, `TaskStatus`, `GoalStatus`, `GoalPeriod`, `ReminderType`, `ReminderRecurrence` e `Shift`.

`Shift` não é campo de nenhuma entidade: o turno de uma tarefa é **derivado** de `TimeSlot::start()`, conforme decidido em [`docs/reporting-metrics-contract.md`](docs/reporting-metrics-contract.md).

## Documentação

Documentos adicionais estão disponíveis na pasta `docs/`:

- [`docs/getting-started.md`](docs/getting-started.md): primeiros passos.
- [`docs/architecture.md`](docs/architecture.md): decisões de arquitetura.
- [`docs/conventions.md`](docs/conventions.md): convenções de código, testes e build.
- [`docs/persistence-architecture.md`](docs/persistence-architecture.md): camada de persistência.
- [`docs/postgresql.md`](docs/postgresql.md): uso do PostgreSQL.
- [`docs/postgresql-integration-report.md`](docs/postgresql-integration-report.md): relatório da integração com PostgreSQL.
- [`docs/api.md`](docs/api.md): contrato JSON, endpoints, erros, CORS e log.
- [`docs/date-timeslot-contract.md`](docs/date-timeslot-contract.md): contrato público congelado de `Date` e `TimeSlot`.
- [`docs/reporting-metrics-data.md`](docs/reporting-metrics-data.md): dados necessários e casos de teste das métricas de relatório.
- [`docs/reporting-metrics-contract.md`](docs/reporting-metrics-contract.md): fórmulas e contrato das métricas de relatório.
- [`back-end/migrations/README.md`](back-end/migrations/README.md): convenção de numeração das migrações.

O planejamento e o estado das tarefas ficam nas issues do GitHub, não neste arquivo.

## 🖥️ Front-end (Interface do Usuário)

O front-end do Virtual Planner foi construído com **React 19, TypeScript, Vite e Tailwind CSS v4**. Hoje ele opera de forma independente do back-end: consome mocks em `front-end/src/mocks`, não a API.

As rotas ficam em `src/App.tsx`, dentro de um `AppShell` com sidebar e alternância de tema:

| Rota | Tela |
| --- | --- |
| `/` | dashboard |
| `/tasks`, `/tasks/new`, `/tasks/:id/edit` | tarefas |
| `/planner` | quadro semanal |
| `/goals`, `/goals/new`, `/goals/:id/edit` | metas |
| `/reminders`, `/reminders/new`, `/reminders/:id/edit` | lembretes |
| `/reports` | painel analítico |
| `/profile`, `/settings` | perfil e ajustes |

### Como rodar o front-end localmente

```bash
cd front-end
npm ci
npm run dev
```

`npm ci` instala exatamente o que está em `package-lock.json` — use `npm install` só quando a intenção for alterar dependências.

### Scripts

| Comando | O que faz |
| --- | --- |
| `npm run dev` | Servidor de desenvolvimento do Vite, com hot reload |
| `npm run build` | `tsc -b` seguido do build de produção do Vite |
| `npm run lint` | ESLint sobre todo o workspace |
| `npm run preview` | Serve localmente o resultado de `npm run build` |

### Integração contínua

`.github/workflows/frontend.yml` roda em Node 22 a cada push em `main` e a cada pull request que toque `front-end/**`, executando `npm ci`, `npm run build` e `npm run lint`. Rode os três localmente antes de abrir PR: o job falha no primeiro erro de tipo ou de lint.

### Estrutura

```text
front-end/src
├── components   # componentes reutilizáveis de UI
├── pages        # telas, uma por rota
├── lib          # helpers sem JSX
├── mocks        # dados de exemplo enquanto a API não é consumida
├── types        # tipos compartilhados entre telas
└── assets       # imagens e estáticos
```

As convenções de front-end estão em [AGENTS.md](AGENTS.md#agent-especialista-frontend--react--typescript) e em [front-end/README.md](front-end/README.md).
