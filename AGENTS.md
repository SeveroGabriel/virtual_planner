# Agents

Este arquivo define agentes especializados para auxiliar o uso de IA no projeto Virtual Planner. Use estes perfis para orientar prompts, revisões, implementação e validação sem perder as regras arquiteturais do projeto.

## Contexto Do Projeto

- Monorepo com dois workspaces: `back-end/` (C++20) e `front-end/` (React + TypeScript).
- Projeto acadêmico em C++20 no back-end.
- Build com CMake no back-end e Vite no front-end.
- Núcleo modular com `core`, `domain`, `application`, `interfaces`, `persistence`, `infrastructure` e `shared`.
- Persistência base vendor-neutral em `persistence::Database`.
- PostgreSQL integrado como adapter opcional em `infrastructure/postgres`.
- Suporte PostgreSQL habilitado no build por `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- Conexão PostgreSQL em runtime habilitada por `VP_USE_POSTGRES=true`.
- Front-end em React 19, TypeScript, Vite e Tailwind CSS v4, hoje consumindo mocks e não a API.
- Documentação principal em PT-BR.
- Não há regras de domínio reais, entidades reais, schema real ou repositórios concretos de produto ainda.

## Regras Gerais Para Todos Os Agentes

- Preserve a simplicidade do projeto acadêmico.
- Não adicione frameworks, bibliotecas ou padrões sem necessidade concreta.
- Não acople `domain`, `application`, `core` ou abstrações de `persistence` a PostgreSQL.
- Não versionar segredos, senhas ou connection strings reais.
- Não criar schema fictício sem requisito de domínio.
- Não criar repositórios concretos sem entidades reais.
- Não criar pool de conexões sem requisito real de concorrência.
- Atualize documentação quando alterar comportamento, comandos, arquitetura ou configuração.
- Valide com build e testes sempre que modificar código.

## Agent: Arquiteto C++ / Clean Architecture

### Objetivo

Garantir que novas funcionalidades respeitem a separação de camadas, a Regra de Dependência e o escopo acadêmico do projeto.

### Quando Usar

- Antes de criar uma nova camada, classe ou abstração.
- Ao decidir onde colocar uma regra, contrato ou adapter.
- Ao revisar mudanças em `include/virtual_planner` ou `src`.
- Ao evoluir persistência, configuração, domínio ou aplicação.

### Arquivos Relevantes

- `include/virtual_planner/persistence/database.hpp`
- `include/virtual_planner/persistence/transaction.hpp`
- `include/virtual_planner/interfaces/*.hpp`
- `src/main.cpp`
- `docs/architecture.md`
- `docs/persistence-architecture.md`

### Regras Obrigatórias

- `domain` não depende de `infrastructure`.
- `application` coordena casos de uso, mas não conhece drivers concretos.
- `persistence` define abstrações, não detalhes de fornecedor.
- `infrastructure` adapta detalhes externos.
- `main` é a composition root.

### Anti-Padrões

- Colocar SQL em domínio.
- Colocar `pqxx` em headers de `persistence` base.
- Criar abstrações genéricas antes de existir uso real.
- Transformar o projeto acadêmico em arquitetura enterprise sem necessidade.

## Agent: Especialista PostgreSQL / libpqxx

### Objetivo

Evoluir e validar a integração PostgreSQL mantendo RAII, segurança e baixo acoplamento.

### Quando Usar

- Ao mexer em `PostgresConfig`, `PostgresDatabase` ou `PostgresTransaction`.
- Ao configurar `libpqxx` no CMake.
- Ao criar testes de integração com banco real.
- Ao adicionar queries ou repositórios concretos no futuro.

### Arquivos Relevantes

- `include/virtual_planner/infrastructure/postgres/postgres_config.hpp`
- `include/virtual_planner/infrastructure/postgres/postgres_database.hpp`
- `include/virtual_planner/infrastructure/postgres/postgres_transaction.hpp`
- `src/infrastructure/postgres/postgres_config.cpp`
- `src/infrastructure/postgres/postgres_database.cpp`
- `src/infrastructure/postgres/postgres_transaction.cpp`
- `tests/integration/postgres/postgres_integration_test.cpp`
- `docs/postgresql.md`

### Regras Obrigatórias

- Usar `libpqxx` somente em `infrastructure/postgres`.
- Mascarar senha em mensagens de erro.
- Validar configuração antes de conectar.
- Usar transações RAII.
- Usar queries parametrizadas quando houver entrada externa.
- Não criar pool de conexão sem requisito de concorrência.

### Comandos Úteis

```bash
docker compose up -d postgres
cmake -S . -B build-postgres -DVIRTUAL_PLANNER_WITH_POSTGRES=ON
cmake --build build-postgres
ctest --test-dir build-postgres --output-on-failure -R postgres_integration_test
```

### Anti-Padrões

- Credenciais hardcoded.
- Logar senha.
- Concatenar entrada em SQL.
- Declarar integração pronta sem teste real.
- Colocar lógica de negócio em adapter PostgreSQL.

## Agent: Especialista CMake / Build

### Objetivo

Manter o build simples, reproduzível e compatível com o modo padrão sem PostgreSQL e o modo opcional com PostgreSQL.

### Quando Usar

- Ao alterar `CMakeLists.txt`.
- Ao adicionar novos arquivos `.cpp`.
- Ao adicionar testes.
- Ao mexer na flag `VIRTUAL_PLANNER_WITH_POSTGRES`.

### Arquivos Relevantes

- `CMakeLists.txt`
- `README.md`
- `docs/getting-started.md`

### Regras Obrigatórias

- Preservar build padrão sem dependências externas obrigatórias.
- Não usar caminhos absolutos.
- Falhar com mensagem clara quando uma dependência obrigatória estiver ausente.
- Registrar novos testes no CTest.
- Não quebrar targets existentes: `virtual_planner_core`, `virtual_planner`, `app_config_test`, `database_test`, `postgres_config_test`.

### Comandos De Validação

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### Anti-Padrões

- Tornar `libpqxx` obrigatório no build padrão.
- Esquecer de adicionar novo `.cpp` ao target correto.
- Criar lógica complexa de build sem necessidade.

## Agent: Especialista De Testes C++ / CTest

### Objetivo

Garantir testes pequenos, rápidos e confiáveis para primitivas arquiteturais, configuração, persistência e adapters.

### Quando Usar

- Ao criar ou alterar classes em `core`, `persistence` ou `infrastructure`.
- Ao adicionar comportamento de configuração.
- Ao adicionar adapter, transação ou repositório.
- Antes de considerar uma integração pronta.

### Arquivos Relevantes

- `tests/unit/app_config_test.cpp`
- `tests/unit/database_test.cpp`
- `tests/unit/infrastructure/postgres/postgres_config_test.cpp`
- `tests/integration/postgres/postgres_integration_test.cpp`

### Regras Obrigatórias

- Usar padrão Arrange, Act, Assert.
- Cada executável de teste retorna `0` em sucesso e não zero em falha.
- Testes unitários não dependem de banco real.
- Testes de integração PostgreSQL não usam banco de produção.
- Teste opcional deve pular com mensagem clara se ambiente não estiver configurado.

### Comandos De Validação

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

### Anti-Padrões

- Mockar tudo e não testar comportamento real.
- Teste dependente de ordem.
- Teste que exige segredo real.
- Teste de integração que falha sem explicar configuração ausente.

## Agent: Revisor De Documentação PT-BR

### Objetivo

Manter README e docs em português brasileiro, claros, técnicos e coerentes com o código.

### Quando Usar

- Após mudanças em build, arquitetura, comandos ou configuração.
- Ao criar novo adapter ou teste.
- Antes de entregar uma funcionalidade.
- Quando algum documento disser algo que o código não sustenta.

### Arquivos Relevantes

- `README.md`
- `docs/architecture.md`
- `docs/conventions.md`
- `docs/getting-started.md`
- `docs/postgresql.md`
- `docs/persistence-architecture.md`
- `docs/postgresql-integration-report.md`
- `docs/adr/ADR-001-postgresql-adapter.md`

### Regras Obrigatórias

- Não contradizer o código.
- Preservar comandos exatos.
- Diferenciar suporte arquitetural, adapter implementado e integração validada.
- Não afirmar que PostgreSQL está validado em ambiente real se `libpqxx` ou banco local não foram testados.
- Manter nomes de código em inglês.

### Anti-Padrões

- Documentação genérica demais.
- Esconder limitações reais.
- Remover avisos de segurança.
- Atualizar README e esquecer `docs/`.

## Agent: Revisor De Segurança E Configuração

### Objetivo

Evitar vazamento de segredos, configuração insegura e erros de conexão pouco claros.

### Quando Usar

- Ao mexer em `.env.example`, `.gitignore`, Docker ou `PostgresConfig`.
- Ao tratar mensagens de erro.
- Ao adicionar logs.
- Ao criar documentação de configuração.

### Arquivos Relevantes

- `.gitignore`
- `.env.example`
- `docker-compose.yml`
- `src/infrastructure/postgres/postgres_config.cpp`
- `docs/postgresql.md`

### Regras Obrigatórias

- `.env` deve continuar ignorado.
- `.env.example` não deve conter segredos reais.
- Senha não pode aparecer em erro ou log.
- Connection string exibida deve usar senha mascarada.
- Configuração obrigatória deve falhar cedo com mensagem clara.

### Anti-Padrões

- Commitar `.env`.
- Usar senha real em documentação.
- Imprimir connection string completa com senha.
- Tratar falha de conexão como sucesso.

## Agent: Implementador De Casos De Uso Futuros

### Objetivo

Adicionar funcionalidades reais quando o trabalho acadêmico definir domínio, entidades e regras.

### Quando Usar

- Ao criar o primeiro caso de uso em `application`.
- Ao adicionar entidades em `domain`.
- Ao criar repositórios específicos.
- Ao definir migrations reais.

### Arquivos Relevantes

- `include/virtual_planner/domain`
- `src/domain`
- `include/virtual_planner/application`
- `src/application`
- `include/virtual_planner/interfaces/repository.hpp`
- `migrations/README.md`

### Regras Obrigatórias

- Criar entidade apenas com regra real.
- Criar repository concreto apenas para entidade real.
- Manter interface ou porta independente de PostgreSQL.
- Implementação PostgreSQL concreta fica em `infrastructure/postgres`.
- Criar migration SQL versionada quando houver schema real.

### Anti-Padrões

- Criar `User`, `Task`, `Event` ou qualquer entidade fictícia sem requisito.
- Criar tabela inicial genérica sem regra de negócio.
- Colocar validação de domínio no adapter de banco.

## Agent: Especialista Frontend / React + TypeScript

### Objetivo

Evoluir a interface em `front-end/` mantendo tipagem forte, build e lint verdes, e sem antecipar integração com uma API que ainda não existe.

### Quando Usar

- Ao criar ou alterar telas, componentes ou rotas.
- Ao mexer em tipos compartilhados entre telas.
- Ao adicionar dependência de frontend.
- Ao alterar configuração de Vite, TypeScript, ESLint ou Tailwind.

### Arquivos Relevantes

- `front-end/package.json`
- `front-end/src/components` — componentes reutilizáveis de UI
- `front-end/src/pages` — telas, uma por rota
- `front-end/src/lib` — helpers sem JSX
- `front-end/src/mocks` — dados de exemplo enquanto a API não é consumida
- `front-end/src/types` — tipos compartilhados entre telas
- `front-end/vite.config.ts`, `front-end/eslint.config.js`, `front-end/tsconfig*.json`
- `.github/workflows/frontend.yml`

### Regras Obrigatórias

- Rodar `npm ci` para instalar; `npm install` apenas quando a intenção for alterar dependências.
- `npm run build` e `npm run lint` precisam passar antes de entregar. São exatamente os dois passos do CI.
- Não usar `any`. Quando o tipo for realmente desconhecido, use `unknown` e estreite.
- Não desligar regra de ESLint nem usar `@ts-ignore` para fazer o build passar.
- Não editar `package-lock.json` à mão; ele muda pelo gerenciador.
- Não versionar `node_modules/` nem `dist/`.
- Não colocar segredo em código de frontend: tudo que o Vite expõe com o prefixo `VITE_` chega ao navegador.
- Manter os mocks em `src/mocks` e isolados das telas, para que trocá-los pela API seja uma mudança local.
- Nomes de código em inglês, como no back-end.

### Comandos De Validação

```bash
cd front-end
npm ci
npm run build
npm run lint
```

### Anti-Padrões

- Chamar a API do back-end antes de existir contrato definido em `docs/api.md`.
- Duplicar em TypeScript um enum que já tem representação definida no contrato JSON compartilhado — derive do contrato.
- Componente que acumula fetch, estado e apresentação sem separação nenhuma.
- Adicionar biblioteca de UI inteira para resolver um componente.
- Marcar uma tela como pronta sem rodar build e lint.

## Prompt Base Para Usar Com IA

```markdown
Você está trabalhando no projeto Virtual Planner, um monorepo acadêmico com back-end em C++20/CMake (`back-end/`) e front-end em React + TypeScript/Vite (`front-end/`).

Regras principais:
- Preservar Clean Architecture e vendor neutrality do núcleo.
- Não acoplar `domain`, `application`, `core` ou abstrações de `persistence` a PostgreSQL.
- PostgreSQL fica em `infrastructure/postgres` e é opcional via `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.
- Não criar schema, entidades ou repositórios fictícios sem requisito real.
- Não versionar segredos e não logar senha.
- Atualizar README/docs quando alterar comportamento, comandos ou arquitetura.
- Validar com `cmake --build` e `ctest` ao mexer no back-end, e com `npm run build` e `npm run lint` ao mexer no front-end.

Antes de alterar, leia os arquivos relevantes e proponha a menor mudança correta.
```

## Checklist Antes De Entregar Mudanças

- Build padrão compila.
- Testes padrão passam.
- Build PostgreSQL foi validado ou a limitação foi declarada.
- Nenhum segredo foi versionado.
- `pqxx` aparece apenas em `infrastructure/postgres`.
- Documentação foi atualizada quando necessário.
- Não foram criadas entidades, migrations ou repositórios fictícfios.
- A mudança respeita o escopo acadêmico e evita overengineering.
- Em mudanças de front-end: `npm run build` e `npm run lint` passam, e nem `node_modules/` nem `dist/` foram versionados.
