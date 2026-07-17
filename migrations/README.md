# Migrações

Este diretório está reservado para scripts SQL versionados do PostgreSQL.

Ainda não há schema inicial porque as entidades atuais ainda não têm mapeamento persistente definido. Quando o primeiro caso de uso exigir persistência real, crie arquivos numerados, por exemplo:

```text
001_initial_schema.sql
002_add_required_index.sql
```

Regras:

- Não criar tabelas fictícias sem caso de uso persistente.
- Manter os scripts reproduzíveis em ambientes locais e de teste.
- Não versionar credenciais ou dados sensíveis.
- Documentar o comando usado para aplicar as migrações quando a estratégia for definida.
