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
- Testes ficam em `tests/unit` ou `tests/integration`.
- Documentação fica em `docs`.
- Adapter PostgreSQL fica em `infrastructure/postgres`.
- Scripts SQL versionados devem ficar em `migrations` quando houver schema real.

## Includes

- Inclua headers do projeto com o prefixo `virtual_planner/...`.
- Mantenha os includes mínimos e locais ao arquivo que os utiliza.
- Não exponha headers de infraestrutura a partir de headers de domínio ou de `core`.
- Não inclua `pqxx` em `domain`, `application`, `core`, `interfaces` ou nos headers base de `persistence`.

## Testes

- Testes unitários são executáveis C++ simples registrados no CTest.
- Cada executável de teste deve retornar um valor diferente de zero em caso de falha.
- Organize cenários no padrão Arrange, Act, Assert.
- Adicione um teste pequeno para cada nova primitiva arquitetural ou adaptador.
- Adicione testes unitários para value objects, entidades e conversões de enum quando introduzir comportamento de domínio.
- Testes de integração PostgreSQL devem depender de banco local descartável ou container, nunca de banco de produção.
- Testes de integração opcionais devem pular com mensagem clara quando o ambiente não estiver configurado.

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
