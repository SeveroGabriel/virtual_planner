# Produção na Railway

Preparação local; este guia não cria recursos nem executa deploys. Documentação
oficial consultada em 31/08/2026. O ambiente Railway real ainda precisa ser
configurado e validado pelo responsável pela publicação.

## Arquitetura e decisões

```text
Internet -- HTTPS --> frontend (nginx + bundle React)
                         |
                         +-- /api/* -- HTTP privado --> backend (C++20)
                                                          |
                                                          +-- TLS --> Postgres
```

Os três serviços ficam no mesmo projeto **e ambiente**. Apenas o frontend
recebe domínio público. Não habilite acesso público/TCP Proxy no banco nem
domínio público no backend. A rede privada Railway é isolada por ambiente e
criptografada; ela só existe em runtime, não no build.
[Referência: rede privada](https://docs.railway.com/networking/private-networking/how-it-works).

O navegador usa `/api`, sem conhecer o DNS privado. `BACKEND_URL` configura
somente o nginx em runtime; não é `VITE_*`. Isso preserva a sessão same-origin
e dispensa cookies cross-site, mudanças de domínio ou `SameSite=None`.

O adapter já aceita todos os parâmetros PostgreSQL necessários. Usamos
referências Railway nas variáveis `POSTGRES_*`, sem novo parser de
`DATABASE_URL`. Nem `DATABASE_URL` nem `FRONTEND_URL` são consumidas diretamente
pelo aplicativo; use o mapeamento abaixo e `VP_HTTP_ALLOWED_ORIGINS`.

Não foram adicionados `railway.json`/`railway.toml`: a documentação atual marca
Config as Code como legado, com migração para IaC. Para manter esta preparação
pequena e sem provisionamento, os ajustes ficam nas imagens e no setup manual.
[Referência: Config as Code](https://docs.railway.com/config-as-code/reference).

## Setup manual

1. No projeto/ambiente desejado, adicione o PostgreSQL oficial e nomeie-o
   `Postgres`. Adicione dois serviços do repositório, `backend` e `frontend`.
   Os nomes precisam coincidir com as referências dos exemplos de variáveis.
2. Em Settings → Source, configure **Root Directory `/back-end`** na API
   e **`/front-end`** no frontend. Ambos usam o mesmo repositório
   `https://github.com/tiagojose76/virtual-planner`. Não publique um serviço
   da aplicação pela raiz: cada Dockerfile usa apenas seu próprio contexto.
3. Configure os Dockerfiles, variáveis e checks da tabela. Gere domínio HTTPS
   somente para `frontend`, para então resolver sua referência de origem.
4. Revise as migrations e configure o pre-deploy do backend antes de publicar.
   Publique o banco primeiro, depois API e frontend. Não use Docker Compose
   como comando de start na Railway.

| Configuração | backend | frontend |
| --- | --- | --- |
| Repository | `tiagojose76/virtual-planner` | `tiagojose76/virtual-planner` |
| Root Directory | `/back-end` | `/front-end` |
| Builder | Dockerfile | Dockerfile |
| Dockerfile Path | `Dockerfile` | `Dockerfile` |
| Port | `PORT=8080`, bind `0.0.0.0` | `PORT=8080`, nginx |
| Start Command | deixar vazio: `virtual_planner` da imagem | deixar vazio: entrypoint oficial nginx |
| Pre-deploy Command | `/app/migrations/db-migrate.sh` | nenhum |
| Pre-deploy timeout | 300 s, ajustar após medir migrations | — |
| Healthcheck Path | `/health` | `/health` |
| Healthcheck timeout | 300 s | 300 s |
| Réplicas | **1** | 1 inicialmente |
| Networking público | nenhum | domínio HTTPS, target port igual a `PORT` |

Não sobrescreva o start do nginx: seu entrypoint renderiza o template com as
variáveis de ambiente. Para detectar mudanças, os caminhos relevantes são
`/back-end/**` na API e `/front-end/**` no frontend; cada pasta inclui seu
próprio `.dockerignore`. Deixar watch patterns sem restrição também funciona.

Remova overrides antigos de Build Command e Start Command, especialmente
`start.sh`, e qualquer `RAILWAY_DOCKERFILE_PATH` apontando para a raiz. Se
configurar essa variável explicitamente, use `RAILWAY_DOCKERFILE_PATH=Dockerfile`
em ambos. O Dockerfile local é detectado após definir a Root Directory.
Confirme no log futuro a detecção do Dockerfile, sem inferência pelo Railpack.

O erro relatado `Script start.sh not found` / `Railpack could not determine
how to build the app` é compatível com um serviço apontando para a raiz
sem Dockerfile. Além disso, os Dockerfiles anteriores dependiam de caminhos
da raiz. Não existe necessidade de criar `start.sh`: o backend usa
`CMD ["virtual_planner"]` e o frontend herda `/docker-entrypoint.sh` com
`nginx -g "daemon off;"` da imagem oficial. A configuração remota não foi acessada.

[Dockerfile customizado](https://docs.railway.com/builds/dockerfiles),
[monorepos](https://docs.railway.com/deployments/monorepo) e
[start commands](https://docs.railway.com/deployments/start-command).

## Variáveis

Os arquivos abaixo são modelos para o editor de variáveis Railway, **não**
arquivos para executar com `source` ou copiar para o `.env` do Compose:

- [Backend](../deploy/railway/backend.env.example)
- [Frontend](../deploy/railway/frontend.env.example)

Backend: `VP_PROFILE=production`, `VP_USE_POSTGRES=true`,
`VP_HTTP_HOST=0.0.0.0` e `PORT=8080`. A porta é configurável; definir seu valor
explicitamente no serviço permite referenciá-la no frontend. Não defina
`VP_HTTP_PORT` na Railway, pois ele tem precedência sobre `PORT`.

| Variável da API e migrations | Referência Railway |
| --- | --- |
| `POSTGRES_HOST` | `${{Postgres.PGHOST}}` |
| `POSTGRES_PORT` | `${{Postgres.PGPORT}}` |
| `POSTGRES_DB` | `${{Postgres.PGDATABASE}}` |
| `POSTGRES_USER` | `${{Postgres.PGUSER}}` |
| `POSTGRES_PASSWORD` | `${{Postgres.PGPASSWORD}}` |

Confirme que `PGHOST` aponta para o serviço privado. Não copie uma senha real
para o repositório. Mantenha `POSTGRES_CONNECT_TIMEOUT=5` (também usado pelo
migrator) e `POSTGRES_APPLICATION_NAME=virtual-planner`.
[Variáveis PostgreSQL](https://docs.railway.com/databases/postgresql).

Frontend: `PORT=8080`, `VITE_API_URL=/api` no build e
`BACKEND_URL=http://${{backend.RAILWAY_PRIVATE_DOMAIN}}:${{backend.PORT}}`
em runtime, **sem `/api` nem barra final**. O nginx preserva caminho e query
string. O resolver do container renova o DNS do backend após mudanças de IP;
o frontend consegue iniciar mesmo antes de o backend estar resolvível.

Ambientes Railway novos têm DNS IPv4 e IPv6; legados podem ter apenas IPv6.
Se o backend precisar atender IPv6, use `VP_HTTP_HOST=::`: o cpp-httplib da
imagem permite IPv4 mapeado nesse socket. `0.0.0.0` continua sendo o padrão
para Docker/local e redes com IPv4. Não substitua o DNS privado por um IP fixo.

### TLS do banco

A Railway oferece um template PostgreSQL com TLS e certificado próprio.
A rede privada já usa WireGuard; TLS PostgreSQL é uma camada adicional e
também atende à política existente do aplicativo em `production`.
Use `POSTGRES_SSLMODE=require` **após confirmar** que o serviço escolhido
usa esse template e está com TLS habilitado. Não alteramos o modo local,
que continua `disable` no Compose. Uma imagem PostgreSQL customizada pode
exigir configuração diferente: não desative a proteção para esconder falhas.

`require` exige criptografia, mas não garante a identidade do servidor.
Para validação de identidade, disponibilize a CA confiável tanto à API quanto
ao pre-deploy, configure `PGSSLROOTCERT` e `POSTGRES_SSLMODE=verify-full` e
confirme que o certificado cobre o DNS privado. Volumes do serviço não são
montados no pre-deploy: a CA precisa estar na imagem ou ser materializada a
partir de configuração segura também nesse container.

Confira a conexão real com `SHOW ssl` e, na própria sessão de banco:
`SELECT ssl FROM pg_stat_ssl WHERE pid = pg_backend_pid();`.
[Imagem oficial Railway](https://github.com/railwayapp-templates/postgres-ssl),
[modos TLS libpq](https://www.postgresql.org/docs/current/libpq-ssl.html).

## Sessões, HTTPS e CORS

`vp_session` já usa `HttpOnly`, `SameSite=Strict`, `Path=/` e não define
`Domain` (host-only). Em `production`, login e logout acrescentam `Secure`.
A Railway termina o HTTPS público; o HTTP privado entre nginx e API não
remove esse atributo, que depende do perfil e não de um header encaminhado.
O cliente React já usa `credentials: "include"`.

Configure `VP_HTTP_ALLOWED_ORIGINS=https://${{frontend.RAILWAY_PUBLIC_DOMAIN}}`.
Com domínio customizado, informe sua origem HTTPS exata; múltiplas origens
separam-se por vírgula, sem caminhos nem barra final. Nunca use `*`.
O middleware ecoa uma origem autorizada e acrescenta
`Access-Control-Allow-Credentials: true`; não devolve wildcard credenciado.

As sessões ficam **em memória**, não no PostgreSQL. Mantenha uma réplica da
API e aceite novo login após restart/redeploy. Contas e dados persistem. O
store atual não implementa expiração por tempo nem limpeza de sessões sem
logout; persistência/expiração de sessões e limitação de tentativas de login
são melhorias separadas antes de ampliar o uso público. Cadastro é aberto.

## Migrations e prontidão

A imagem da API inclui `psql`, `back-end/migrations/db-migrate.sh` e os SQLs
versionados, instalados em `/app/migrations`. O migrador pertence ao backend
e não depende de arquivos fora de `/back-end`. O comando local/CI
`./scripts/db-migrate.sh` permanece como wrapper para esse mesmo script,
sem duplicar lógica nem alterar os SQLs.
O script só roda quando invocado; não há migração automática no start nem
conexão ao banco durante o Docker build. O pre-deploy usa as mesmas variáveis
do runtime, executa migrations pendentes em ordem e registra cada uma em
transação. Erro retorna código não zero e deve impedir a nova publicação.

Não há novo SQL destrutivo nesta preparação. Revise migrations futuras,
mantenha backup e serialize deploys que migram o mesmo banco. O controle
atual não implementa lock global entre dois migrators simultâneos. Rollback
da aplicação não desfaz migrations: mantenha compatibilidade com a versão
anterior enquanto ela ainda estiver servindo tráfego.
[Pre-deploy Railway](https://docs.railway.com/deployments/pre-deploy-command).

- API `/health`: 200 pronta, 503 com banco indisponível; em produção, banco
  não configurado também impede prontidão. A checagem usa `SELECT 1`.
- API `/api/health`: contrato anterior preservado, sempre 200 e JSON
  `ok`/`degraded`. Não usar como readiness Railway.
- Frontend `/health`: comprova que nginx iniciou; independe da API. Verifique
  também `/api/health` pelo domínio público para testar o caminho completo.

A readiness não valida schema; a aplicação pressupõe pre-deploy concluído.
A Railway usa HTTP 200 para promover um deploy, mas não monitora esse check
continuamente depois. A API atual não reconecta automaticamente após perder
o banco: restaure o banco, reinicie a API e faça login novamente.
[Healthchecks Railway](https://docs.railway.com/deployments/healthchecks).

## Validação local

```bash
cd front-end
npm ci
npm run lint
npm test
npm run build # inclui tsc -b
npm run format:check
cd ..
cmake -S back-end -B back-end/build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build back-end/build-debug --parallel 2
ctest --test-dir back-end/build-debug --output-on-failure
docker build -t virtual-planner-backend ./back-end
docker build -t virtual-planner-frontend ./front-end
```

Para integração, use um banco **descartável**, aplique migrations pela imagem
e configure o build com `VIRTUAL_PLANNER_WITH_HTTP=ON` e
`VIRTUAL_PLANNER_WITH_POSTGRES=ON`. Exporte suas `POSTGRES_*` antes de rodar
CTest para que os testes PostgreSQL não sejam pulados.

Use portas diferentes de 8080/80 nas duas imagens para verificar `PORT`.
Para exercitar cookies `Secure`, coloque um proxy TLS local na frente do
frontend e use uma CA de teste confiável, sem desativar verificação TLS:

```bash
SSL_CERT_FILE=/caminho/ca-de-teste.crt \
  python3 scripts/smoke-api.py --base-url https://localhost:19443
```

O smoke existente exercita cadastro/login/logout, isolamento de dois usuários,
CRUD, conflitos e relatórios. Complemente com: atributos do cookie, CORS exato,
SPA `/planner`, assets, execução repetida de migrations sem duplicação,
prontidão 503 após queda do banco e novo login com dados preservados após
restart. Não rode testes de escrita contra o banco de aula ou produção.

O `docker-compose.yml` usa `context: ./back-end` na API e `context: ./front-end`
no web, ambos com `dockerfile: Dockerfile`. O serviço local `migrate` continua
executando o wrapper e usando o banco separado. Seus padrões continuam
`api:8080`, nginx em 80 (publicado em 8081) e perfil development. Não altere
o `.env` local para testar produção; use outra stack e outro volume.

### Validação dos contextos independentes — 31/08/2026

Executada localmente em containers Linux/ARM64, sem acessar a Railway:

| Verificação | Resultado |
| --- | --- |
| `docker build -t virtual-planner-backend ./back-end` | PASS |
| `docker build -t virtual-planner-frontend ./front-end` | PASS — inclui TypeScript/Vite |
| Startup API com `PORT=19080` e bind `0.0.0.0` | PASS — `/health` 200 com PostgreSQL conectado |
| Startup nginx com `PORT=19081` | PASS — `/health`, SPA e assets |
| `docker compose up -d --build --wait` | PASS — projeto, banco e portas descartáveis |
| Smoke existente via Compose e via proxy em portas dinâmicas | PASS — auth, CRUD, isolamento e relatórios |
| Wrapper local e migrador incluído na imagem | PASS — 12 migrations aplicadas, segunda execução sem reaplicar |
| Scripts sem `POSTGRES_PASSWORD` | PASS — falham com código 1 e mensagem explícita |
| `bash -n` e `git diff --check` | PASS |

O Compose foi testado com override apenas de tags de imagem e portas do host
(25432, 28080, 28081), mantendo as portas internas padrão. Os containers
avulsos usaram 19080/19081 para comprovar `PORT`, sem comando de start customizado.
O teste avulso precisou reiniciar o nginx após anexá-lo à rede Docker para
atualizar seu resolver; em uso normal, conecte a rede antes de iniciar o serviço.
Nesta correção, o runtime foi validado em `development`; os testes HTTPS/TLS
da preparação anterior estão separados abaixo. Não houve nova execução das
suítes unitárias, alteração de C++/React, deploy ou validação na Railway real.

### Histórico da preparação inicial — 31/08/2026

Resultados da preparação anterior, antes da separação dos contextos Docker,
em macOS/ARM64 e containers Linux/ARM64, sem acesso à Railway:

| Verificação executada | Resultado |
| --- | --- |
| Frontend `npm ci`, lint, testes, build (`tsc -b`) e formatação | PASS — 27 testes |
| Backend CMake Debug sem PostgreSQL | PASS — 36 testes |
| Backend CMake Release com HTTP + PostgreSQL real/TLS | PASS — 52 testes, nenhum pulado |
| Docker build API e frontend | PASS |
| Smoke integrado HTTPS → nginx → API production → PostgreSQL TLS | PASS — 43 verificações |
| Smoke com Compose local e novas imagens em portas isoladas | PASS — 43 verificações |
| Cookies Secure/HttpOnly/Strict/host-only, CORS, SPA e assets | PASS |
| Migrations pela imagem e segunda execução idempotente | PASS — 12 aplicadas, depois 0 reaplicadas |
| Conta/tarefa após restart, com novo login | PASS |
| Banco parado: readiness 503 e liveness 200/degraded | PASS |
| Frontend antes do DNS/backend existir e API em socket IPv6 dual-stack | PASS |
| Deploy, domínio, TLS e DNS reais da Railway | NOT RUN — fora do escopo autorizado |

O teste novo de prontidão falhou antes da implementação e passou depois.
As suítes completas passaram com a correção. O build macOS emitiu o aviso de
biblioteca duplicada do linker, sem impedir compilação ou testes. Nenhuma
dependência de frontend ou lockfile foi alterada. Não houve teste de carga,
escala horizontal nem garantia de zero downtime para sessões em memória.

## Antes da publicação real

- Configurar os três serviços, referências, HTTPS, pre-deploy e health checks.
- Confirmar TLS, backups, restauração e compatibilidade do schema real.
- Conferir sessão no navegador pelo domínio final e ausência de acesso público
  direto ao banco/API; não transportar contas ou dados fictícios de aula.
- Decidir sobre cadastro aberto e limites de tráfego antes de divulgar o site.
- Repetir o smoke em staging Railway. O teste local não comprova execução na
  infraestrutura Railway, que não foi acessada nesta preparação.
