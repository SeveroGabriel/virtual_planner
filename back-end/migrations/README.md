# Migrações

Este diretório contém os scripts SQL versionados do PostgreSQL, aplicados na ordem numérica do prefixo do arquivo, por exemplo:

```text
001_create_goals_table.sql
002_add_goals_timestamps_and_checks.sql
```

## Convenção de numeração

O nome do arquivo segue `NNN_nome_em_snake_case.sql`, com **três dígitos**. O
prefixo `NNN` é a versão registrada em `schema_migrations` e define a ordem de
aplicação.

Como quatro pessoas criam migrations em paralelo, cada domínio tem uma **faixa
reservada**. Escolha sempre o menor número livre **dentro da sua faixa**:

| Faixa | Domínio | Responsável |
| --- | --- | --- |
| 001–019 | Base e infraestrutura (tabela `users`, extensões, tabelas compartilhadas) | Arquitetura |
| 020–029 | `Goal` | Dani |
| 030–039 | `Task` | Bel |
| 040–049 | `Reminder` | Laysa |
| 050–059 | `User` (dados de perfil além da linha semeada) | Gabriel |
| 060–069 | Relatórios e visões de leitura | Arquitetura |
| 070–099 | Reservado para uso futuro | — |

Assim, duas pessoas de domínios diferentes nunca disputam o mesmo número, e a
ordem de aplicação fica determinada mesmo quando os PRs entram fora de ordem.

### Migrations anteriores à convenção

`001_create_goals_table.sql` e `002_add_goals_timestamps_and_checks.sql` são
anteriores a esta convenção e permanecem na faixa base. Elas **não são
renumeradas**: renomear uma migration já aplicada quebraria o registro em
`schema_migrations`. Mudanças novas em `goals` usam a faixa 020–029.

### Regra de conflito

Se dois PRs abertos usarem o mesmo prefixo:

1. O PR que for mergeado por último renomeia o próprio arquivo para o próximo
   número livre **da sua faixa**, antes do merge. Quem já mergeou não mexe mais.
2. A renomeação só é permitida enquanto a migration não tiver sido aplicada em
   nenhum ambiente compartilhado. Se já foi aplicada, crie uma migration nova.
3. O runner falha de propósito quando encontra dois arquivos com o mesmo
   prefixo, para que a colisão apareça no CI e não vire ordem indefinida em
   produção. `version` também é PRIMARY KEY em `schema_migrations`, então a
   colisão nunca passa despercebida.

## Como aplicar

Use o runner `scripts/db-migrate.sh` (na raiz do repositório) para aplicar as migrações pendentes no PostgreSQL configurado por variáveis de ambiente:

```bash
./scripts/db-migrate.sh
```

O script é idempotente: mantém uma tabela de controle `schema_migrations` e não reaplica migrações já registradas. Cada migração roda em transação; se uma falhar, o script para imediatamente e nada é registrado como aplicado. Veja `scripts/db-migrate.sh` para as variáveis de ambiente aceitas (`POSTGRES_HOST`, `POSTGRES_PORT`, `POSTGRES_DB`, `POSTGRES_USER`, `POSTGRES_PASSWORD`, `POSTGRES_SSLMODE`).

Regras:

- Não criar tabelas fictícias sem caso de uso persistente.
- Manter os scripts reproduzíveis em ambientes locais e de teste.
- Não versionar credenciais ou dados sensíveis.
- Migrações são numeradas e aplicadas em ordem; não editar uma migração já aplicada em qualquer ambiente compartilhado — crie uma nova.
