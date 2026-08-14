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

## Decisões Registradas

### ADR-001 — Destino das portas em `interfaces` (P-13)

**Contexto.** `interfaces/` continha cinco portas sem nenhum consumidor no
código: `cache.hpp`, `event_bus.hpp`, `logger.hpp`, `repository.hpp` e
`serializer.hpp`. Apenas `config_provider.hpp` era realmente incluído (por
`infrastructure/config/environment_config_loader.hpp`). Abstrações
especulativas contrariam a regra do projeto de não criar infraestrutura além do
necessário e aumentam o custo de manutenção sem benefício.

**Decisão.** Manter em `interfaces/` somente portas com consumidor real no
código hoje. Uma porta apenas prevista por issue futura não permanece no
repositório: ela é reintroduzida junto do primeiro consumidor real.

| Porta | Destino | Justificativa |
| --- | --- | --- |
| `config_provider.hpp` | mantida | Consumida por `infrastructure/config/environment_config_loader.hpp`. |
| `logger.hpp` | removida | Sem consumidor no código e não registrada em nenhum alvo CMake. P-54 condiciona a porta a esta ADR ("implementação concreta da porta `Logger`, se mantida em P-13"), de modo que mantê-la aqui seria um adiamento circular. P-54 a reintroduz junto do seu primeiro consumidor real. |
| `cache.hpp` | removida | Sem consumidor e sem issue no backlog que exija cache. |
| `event_bus.hpp` | removida | Sem consumidor e sem requisito de mensageria no escopo. |
| `serializer.hpp` | removida | P-29.0 define a serialização JSON com funções livres sobre `to_string`/`from_string`, não com uma porta genérica. |
| `repository.hpp` | removida | Os contratos de persistência reais são os de `persistence/*_repository.hpp`, que não derivam do template genérico; P-25 implementa aqueles contratos. |

**Consequências.** Se uma dessas portas voltar a ser necessária, ela deve ser
reintroduzida junto do primeiro consumidor real, no mesmo PR, nunca antes.

### ADR-002 — Modelo de usuário: usuário único com `user_id` explícito (P-22A)

**Contexto.** O domínio tem a entidade `User`, mas nenhuma tabela tem `user_id`
e não existe nenhum requisito de autenticação no backlog. Ao mesmo tempo, quatro
pessoas vão escrever migrations de `Goal`, `Task`, `Reminder` e `User` na Onda 3.
Se `user_id` entrar depois, as quatro precisam refazer schema e backfill.

**Decisão.** O sistema é **single-tenant**: existe exatamente um usuário, sem
cadastro, login, senha ou sessão. Ainda assim, as tabelas de dados do usuário
nascem com `user_id` explícito, apontando para esse usuário único semeado pela
migration base.

**Impacto por entidade.**

| Entidade | Impacto |
| --- | --- |
| `User` | Tabela `users` criada na faixa base, com uma linha semeada de `id = 1`. Sem coluna de senha e sem coluna de credencial. `email` continua sendo apenas dado de perfil. |
| `Goal` | A migration de Goal (P-26.1) adiciona `user_id BIGINT NOT NULL REFERENCES users(id)`. A tabela `goals` atual, criada em `001`, não é reescrita: a coluna entra por migration nova, na faixa de Goal. |
| `Task` | Migration de Task (P-26.2) já nasce com `user_id BIGINT NOT NULL REFERENCES users(id)`. |
| `Reminder` | Migration de Reminder (P-26.3) já nasce com `user_id BIGINT NOT NULL REFERENCES users(id)`. |

**Impacto em endpoints e autenticação.**

- Não há autenticação, nem middleware de sessão, nem token.
- O usuário corrente é resolvido no composition root como o usuário único e
  injetado nos casos de uso; nenhum endpoint recebe `user_id` pelo cliente.
- `GET /api/users/me` (P-33) devolve esse usuário único.
- Endpoints de `Goal`, `Task` e `Reminder` não expõem `user_id` no payload nesta
  fase; o filtro por usuário é aplicado no servidor.

**Alternativa rejeitada.** Multiusuário com autenticação real: exigiria senha,
hash, sessão/token e telas de cadastro, tudo fora do escopo acadêmico do
projeto e sem issue no backlog que o cubra.

**Alternativa rejeitada.** Single-tenant sem `user_id` nenhum: mais simples
agora, mas transformaria qualquer evolução para multiusuário em reescrita de
schema e backfill nas quatro tabelas — exatamente o risco que esta decisão
existe para eliminar.

**Consequências.** Nenhuma issue de Onda 3 fica bloqueada esperando decisão de
identidade; a porta para multiusuário fica aberta ao custo de uma coluna e uma
foreign key por tabela.

## Limitações Atuais

- Não há pool de conexões.
- Não há services de aplicação completos.
- Não há repositórios concretos PostgreSQL para entidades de domínio.
- Não há schema SQL real.
- Não há migrations aplicáveis porque ainda não há mapeamento persistente definido.
- O build com PostgreSQL depende de `libpqxx` disponível no ambiente.
