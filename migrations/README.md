# Migrações

Este diretório está reservado para scripts SQL versionados do PostgreSQL.

Ainda não há schema inicial porque o projeto não possui entidades de domínio ou regras de negócio definidas. Quando o primeiro caso de uso exigir persistência real, crie arquivos numerados, por exemplo:

```text
001_initial_schema.sql
002_add_required_index.sql
```

Regras:

- Não criar tabelas fictícias sem requisito de domínio.
- Manter os scripts reproduzíveis em ambientes locais e de teste.
- Não versionar credenciais ou dados sensíveis.
- Documentar o comando usado para aplicar as migrações quando a estratégia for definida.
