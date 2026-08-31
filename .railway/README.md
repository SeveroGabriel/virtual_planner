# Configuração Railway

`railway.ts` descreve `backend`, `frontend` e `Postgres` no mesmo ambiente.
Cada aplicação usa seu próprio contexto Docker. Este diretório não é um
serviço e não deve ser configurado como Root Directory de uma aplicação.
`Postgres` representa o banco existente, sem declarar outra imagem, versão ou
volume. Não é um provisionador de banco novo: se ele ainda não existir, use
o setup manual do guia após autorização e importe sua configuração real.

## Validar sem acessar Railway

Com Node.js 22 ou superior, na raiz do repositório:

```bash
npm ci --prefix .railway
npm run typecheck --prefix .railway
```

Esses comandos não fazem login, não criam recursos e não executam deploy.
O SDK é fixado e isolado do frontend. Não existe `start.sh` nem Dockerfile raiz.

## Aplicação futura em um projeto existente

**Não aplique este arquivo diretamente sem comparar com o ambiente real.**
A IaC Railway trata recursos e variáveis omitidos como exclusões. Os nomes
devem corresponder aos serviços reais. Não substitua o PostgreSQL existente,
não recrie volumes e não aceite exclusões de dados para corrigir um build.

Após autorização para acessar/configurar Railway:

1. Use Railway CLI 5.42.1 ou superior, autentique e vincule o projeto e ambiente
   corretos. Confirme os nomes `backend`, `frontend` e `Postgres`.
2. Importe a configuração atual em uma cópia temporária do checkout com
   `railway config pull`, sem `--include-variables`. Preserve os recursos,
   volumes e variáveis existentes; use `preserve()` para valores secretos.
   Adapte este arquivo ao resultado importado antes de prosseguir.
3. Confirme TLS no PostgreSQL, configure domínio HTTPS somente no frontend
   e mantenha API/banco privados. Consulte [o guia](../docs/railway.md).
4. Na raiz, execute `railway config plan`. Revise que os serviços de aplicação
   usam `/back-end` e `/front-end`, builder `DOCKERFILE`, Dockerfile `Dockerfile`
   e nenhum override de start. Não prossiga se houver exclusões inesperadas.
5. Somente após aprovar o plano e autorizar os efeitos remotos, execute
   `railway config apply` interativamente. Não use flags que confirmem
   exclusões automaticamente. A aplicação pode disparar novos deployments.

O arquivo não é aplicado automaticamente por commit/push. A restrição de não
fazer ações remotas significa que a correção dos settings Railway permanece
pendente, mesmo com a validação local passando.

## Validação desta configuração

Em 31/08/2026: instalação reproduzível com `npm ci`, typecheck e avaliação
offline do arquivo pelo SDK 3.11.0 passaram. Foram conferidos os três recursos,
Root Directories, builders, resets de build/start, health checks, pre-deploy
e correspondência das variáveis com os exemplos de `deploy/railway/`.
O nó do banco não define imagem, versão, senha ou volume.

Isso não é um plano remoto: `railway config plan` e `apply` não foram
executados. Nenhum teste local comprova o estado atual do Dashboard.

Referências: [IaC](https://docs.railway.com/infrastructure-as-code),
[DSL](https://docs.railway.com/infrastructure-as-code/reference),
[monorepos](https://docs.railway.com/deployments/monorepo).
