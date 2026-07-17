# Arquitetura

## Objetivo

Este projeto é uma base modular de backend em C++20 para um trabalho acadêmico. O código atual define estrutura, contratos, configuração, ciclo de vida de persistência, adapter opcional para PostgreSQL, pontos de entrada para testes e uma primeira modelagem de domínio para planejamento pessoal.

A arquitetura deve continuar pragmática: manter limites claros, mas evitar frameworks, padrões ou infraestrutura além do que o trabalho realmente precisa.

## Camadas

- `core`: primitivas usadas por toda a aplicação, como perfil de execução e configuração imutável de inicialização.
- `domain`: entidades, value objects, enums e regras básicas de domínio. Não deve depender de infraestrutura nem de detalhes de persistência.
- `application`: espaço reservado para casos de uso e serviços de aplicação que coordenam operações do domínio por meio de interfaces.
- `interfaces`: portas estáveis usadas pelas camadas internas para evitar acoplamento com tecnologias concretas.
- `persistence`: abstrações do ciclo de vida de armazenamento, transação e contratos relacionados. Não deve conhecer PostgreSQL.
- `infrastructure`: adaptadores para variáveis de ambiente, PostgreSQL, logging, caches e outros detalhes externos.
- `shared`: primitivas transversais que não pertencem a uma camada específica, como exceções base.

## Regra de Dependência

As dependências devem apontar para dentro, na direção das políticas mais estáveis:

- `domain` não deve incluir `infrastructure`.
- `application` pode depender de `domain`, `core` e interfaces estáveis, mas não de adaptadores concretos de infraestrutura.
- `core` não deve incluir cabeçalhos de bancos de dados ou drivers concretos.
- `persistence::Database` e `persistence::Transaction` não devem incluir headers de PostgreSQL.
- `interfaces` deve permanecer pequena e estável.
- `infrastructure` pode depender de `core`, `interfaces`, `persistence` e `shared` para adaptar detalhes externos.
- `main` é a raiz de composição, onde as implementações concretas são conectadas.

## Diagrama De Dependências

```text
domain/application
      |
      v
interfaces + persistence abstractions
      ^
      |
infrastructure/postgres
      |
      v
libpqxx/libpq
```

## Persistência

`virtual_planner::persistence::Database` gerencia apenas estado de ciclo de vida: inicialização, conexão, encerramento e transições de falha. Ela intencionalmente não conhece nenhuma regra de produto nem fornecedor de banco de dados.

`virtual_planner::persistence::Transaction` define o contrato mínimo para `commit()` e `rollback()`.

Os contratos `UserRepository`, `TaskRepository`, `GoalRepository` e `ReminderRepository` definem portas de persistência para entidades do domínio. Eles não representam persistência concreta e não devem conter detalhes de PostgreSQL.

`virtual_planner::infrastructure::postgres::PostgresDatabase` é o adapter concreto para PostgreSQL. Ele herda de `Database`, usa `PostgresConfig`, encapsula a conexão `libpqxx` e só é compilado quando `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.

`virtual_planner::infrastructure::postgres::PostgresTransaction` encapsula `pqxx::work`, faz `commit()`, `rollback()` e aborta automaticamente no destrutor se a transação ainda estiver ativa.

## Domínio

A primeira modelagem de domínio cobre os conceitos principais do Virtual Planner:

- `User`: usuário do sistema.
- `Task`: tarefa planejada com data, horário, prioridade e status.
- `Goal`: meta com categoria, período e status.
- `Reminder`: lembrete com data, horário, tipo e recorrência.
- `Date`: value object para datas válidas.
- `TimeSlot`: value object para intervalos de horário e detecção de sobreposição.

Essa camada ainda não deve conhecer banco de dados, variáveis de ambiente, interface de usuário ou PostgreSQL.

## Configuração

O adaptador `EnvironmentConfigLoader` lê:

- `VP_APP_NAME`, com valor padrão `virtual-planner`.
- `VP_PROFILE`, com valor padrão `development`.

O adapter PostgreSQL lê:

- `POSTGRES_HOST`, com valor padrão `localhost`.
- `POSTGRES_PORT`, com valor padrão `5432`.
- `POSTGRES_DB`, obrigatório.
- `POSTGRES_USER`, obrigatório.
- `POSTGRES_PASSWORD`, obrigatório.
- `POSTGRES_SSLMODE`, com valor padrão `disable`.
- `POSTGRES_CONNECT_TIMEOUT`, com valor padrão `5`.
- `POSTGRES_APPLICATION_NAME`, com valor padrão `virtual-planner`.

## Dependências Externas

O build padrão não exige dependências externas de runtime ou de teste. A integração PostgreSQL usa `libpqxx` apenas quando `VIRTUAL_PLANNER_WITH_POSTGRES=ON`.

Adicione novas bibliotecas externas apenas quando houver uma necessidade concreta e o tradeoff estiver documentado. Para o escopo acadêmico atual, prefira a STL e pequenas abstrações locais.

## Limitações Atuais

- Não há pool de conexões.
- Não há services de aplicação completos.
- Não há repositórios concretos PostgreSQL para entidades de domínio.
- Não há schema SQL real.
- Não há migrations aplicáveis porque ainda não há mapeamento persistente definido.
- O build com PostgreSQL depende de `libpqxx` disponível no ambiente.
