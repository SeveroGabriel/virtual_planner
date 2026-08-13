# Migrações

Este diretório contém os scripts SQL versionados do PostgreSQL, aplicados na ordem numérica do prefixo do arquivo, por exemplo:

```text
001_create_goals_table.sql
002_add_goals_timestamps_and_checks.sql
```

Ao adicionar uma nova migração real (com caso de uso de persistência definido), use o próximo prefixo numérico disponível, por exemplo `003_nome_da_mudanca.sql`.

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
