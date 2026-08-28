# Convenções

## Linguagem

- Use C++20.
- Prefira recursos da biblioteca padrão antes de adicionar dependências.
- Mantenha nomes de código em inglês.
- Use RAII para recursos externos, como conexões e transações.

## Nomenclatura

- Namespaces usam `snake_case`: `virtual_planner::persistence`.
- Tipos usam `PascalCase`: `AppConfig`, `Database`, `PostgresDatabase`.
- Funções e variáveis usam `snake_case`: `parse_execution_profile`.
- Constantes devem ter nomes descritivos e evitar abreviações.

## Arquivos

- Headers públicos ficam em `include/virtual_planner`.
- Implementações ficam em `src` e espelham a estrutura pública de namespaces.
- Entidades ficam em `domain/entities`.
- Value objects ficam em `domain/value_objects`.
- Enums de domínio ficam em `domain/enums`.
- Interfaces de repositório de entidades ficam em `include/virtual_planner/persistence`.
- Serialização JSON de tipos compartilhados fica em `api/json`, nunca em `domain`.
- Servidor HTTP e rotas ficam em `api/http`. `httplib` e `nlohmann/json` podem
  aparecer em `api`, nunca em `domain`, `application`, `core` ou nos contratos
  base de `persistence`.
- Testes ficam em `tests/unit` ou `tests/integration`.
- Documentação fica em `docs`.
- Adapter PostgreSQL fica em `infrastructure/postgres`.
- Scripts SQL versionados devem ficar em `migrations` quando houver schema real.

## Arquivos na raiz do repositório

A raiz é reservada a arquivos de configuração do projeto (`README.md`,
`AGENTS.md`, `.gitignore`, `docker-compose.yml`). Não deixe material avulso ali:
em um checkout novo o `git status` deve ficar limpo, sem arquivos untracked.

- Material da disciplina que sirva de referência para o projeto (enunciado, guia
  de fluxo do time) é versionado em `docs/`, com nome descritivo e sem espaços.
- Anotações pessoais, exports temporários e binários grandes não entram no
  repositório. Mantenha-os fora da árvore de trabalho ou sob um caminho já
  ignorado.
- Hoje nenhum PDF da disciplina está versionado. Ao adicionar o primeiro,
  coloque-o em `docs/`, nunca na raiz.

### Ferramentas locais de agentes

`.claude/`, `.agents/` e `.tokensave/` são configuração e cache de ferramentas de
IA, específicos da máquina de cada pessoa, e ficam ignorados pelo Git:

- `.claude/` e `.agents/`: prompts, definições de agente e permissões locais.
  Variam por pessoa e por ferramenta; versioná-los geraria conflito a cada PR.
- `.tokensave/`: índice de código em SQLite, regenerável e grande demais para o
  histórico.

Se o time decidir compartilhar alguma dessas configurações, versione o arquivo
específico com uma exceção explícita (`!`) no `.gitignore` e registre a decisão
aqui — nunca o diretório inteiro.

## Includes

- Inclua headers do projeto com o prefixo `virtual_planner/...`.
- Mantenha os includes mínimos e locais ao arquivo que os utiliza.
- Não exponha headers de infraestrutura a partir de headers de domínio ou de `core`.
- Não inclua `pqxx` em `domain`, `application`, `core`, `interfaces` ou nos headers base de `persistence`.

## Testes

- Testes unitários são executáveis C++ simples registrados no CTest.
- `VP_EXPECT(condicao, "mensagem")`, de `back-end/tests/support/expect.hpp`, é o
  **único** mecanismo de asserção do projeto. Não crie helper `expect()` local e
  não use `assert()`: `assert()` é removido pelo compilador quando `NDEBUG` está
  definido (`CMAKE_BUILD_TYPE=Release`) e faria o teste passar sem verificar nada.
- Inclua o helper sempre como `#include "support/expect.hpp"`.
- Não introduza framework de teste externo (GoogleTest, Catch2, doctest) sem ADR.
- Cada executável de teste deve retornar um valor diferente de zero em caso de falha.
- Organize cenários no padrão Arrange, Act, Assert.
- Adicione um teste pequeno para cada nova primitiva arquitetural ou adaptador.
- Adicione testes unitários para value objects, entidades e conversões de enum quando introduzir comportamento de domínio.
- Testes de integração PostgreSQL devem depender de banco local descartável ou container, nunca de banco de produção.
- Testes de integração opcionais devem pular com mensagem clara quando o ambiente não estiver configurado.

## Cobertura de testes

O build padrão não mede cobertura. Para medir localmente:

```bash
cmake -S back-end -B back-end/build-coverage \
  -DCMAKE_BUILD_TYPE=Debug -DVIRTUAL_PLANNER_WITH_COVERAGE=ON -DVIRTUAL_PLANNER_WITH_HTTP=ON
cmake --build back-end/build-coverage
ctest --test-dir back-end/build-coverage --output-on-failure
gcovr --root back-end --exclude 'back-end/tests/' --exclude '.*/_deps/.*' --print-summary back-end/build-coverage
```

`VIRTUAL_PLANNER_WITH_COVERAGE=ON` desliga a otimização e instrumenta a
biblioteca e os testes. Os flags são `PUBLIC` porque quem linka
`virtual_planner_core` também precisa do runtime de `gcov`.

O CI roda isso a cada PR no job **Cobertura de testes**: o percentual aparece no
resumo do job, e o relatório HTML fica como artefato por 14 dias. Testes e
dependências baixadas por `FetchContent` ficam fora da medição.

### Meta mínima

**Proposta: 85% de linhas em `back-end/src`, ainda pendente de acordo da
equipe.**

A medição atual, no build padrão sem HTTP, é de **95,2%** (1523 de 1599 linhas).
A proposta de 85% deixa margem para código novo entrar acompanhado de teste sem
travar PR por variação pequena, e ainda assim acusa uma queda real.

O CI **não falha** por cobertura baixa — a issue P-52 pede visibilidade primeiro,
gate depois. Transformar a meta em gate é uma decisão separada, e só faz sentido
depois que a equipe ratificar o número.

## Build modular do backend

O `back-end/CMakeLists.txt` é apenas o ponto de composição. Fontes e testes são
registrados em módulos dentro de `back-end/cmake/`, para que duas pessoas
trabalhando em módulos diferentes nunca editem a mesma linha:

- `cmake/helpers.cmake`: funções compartilhadas (`virtual_planner_add_sources`,
  `virtual_planner_add_test`, `virtual_planner_enable_warnings`).
- `cmake/sources/*.cmake`: fontes da biblioteca `virtual_planner_core`, por
  camada (`core`, `domain`, `application`, `infrastructure`, `postgres`).
- `cmake/tests/*.cmake`: alvos de teste, por módulo (`core`, `infrastructure`,
  `goals`, `tasks`, `reminders`, `users`, `postgres`, `persistence`, `api`).
- `cmake/json.cmake` e `cmake/http.cmake`: dependências externas opcionais
  (`nlohmann/json` e `cpp-httplib`), cada uma atrás da sua opção e desligadas
  por padrão.

### Como registrar um arquivo de código novo

Adicione o caminho, relativo a `back-end/src`, no módulo correspondente em
`cmake/sources/`:

```cmake
virtual_planner_add_sources(domain/entities/task.cpp)
```

### Como registrar um teste novo

Adicione uma linha no módulo do seu domínio em `cmake/tests/`, com o caminho
relativo a `back-end/tests`:

```cmake
virtual_planner_add_test(task_test unit/domain/entities/task_test.cpp)
```

A função já liga o alvo a `virtual_planner_core`, aplica os warnings padrão,
registra o teste no CTest com o mesmo nome e coloca `back-end/tests` no include
path. Por isso, use sempre `#include "support/expect.hpp"`, nunca caminhos
relativos do tipo `../../support/expect.hpp`.

## Arquitetura

- Mantenha o código de domínio independente de banco de dados, sistema de arquivos, variáveis de ambiente e detalhes de framework.
- Coloque services de caso de uso em `application`; não use entidades como orquestradores de fluxo de aplicação.
- Prefira portas em `interfaces` e adaptadores concretos em `infrastructure`.
- Mantenha `main` como a raiz de composição para conectar implementações concretas.
- Não crie pool de conexões sem requisito real de concorrência.
- Não crie schema, migrations ou repositórios concretos antes de existir caso de uso persistente.
- Não coloque SQL, `pqxx` ou detalhes de connection string em `domain`, `application` ou contratos base de `persistence`.

## Segurança

- Não versionar senhas.
- Não imprimir senha em logs ou mensagens de erro.
- Usar `.env.example` apenas com valores de exemplo.
- Manter `.env` ignorado pelo Git.
- Mascarar connection strings em erros e diagnósticos.
