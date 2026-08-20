# Primeiros Passos

## Pré-requisitos

- Compilador com suporte a C++20.
- CMake 3.20 ou mais recente.
- `libpqxx` apenas para compilar com PostgreSQL.
- Docker opcional para subir PostgreSQL local.

## Build Padrão

```bash
cmake -S . -B build
cmake --build build
```

## Testes Padrão

```bash
ctest --test-dir build --output-on-failure
```

Os testes padrão atuais cobrem configuração da aplicação, ciclo de vida base de persistência e configuração PostgreSQL. Testes unitários específicos do domínio devem ser adicionados junto com a evolução das entidades, value objects e services.

## Execução Padrão

```bash
./build/virtual_planner
```

Variáveis de ambiente opcionais:

```bash
VP_APP_NAME=virtual-planner VP_PROFILE=development ./build/virtual_planner
```

## Build Com PostgreSQL

```bash
cmake -S . -B build-postgres -DVIRTUAL_PLANNER_WITH_POSTGRES=ON
cmake --build build-postgres
```

Esse build exige `libpqxx`. Se a dependência não estiver disponível, o CMake falha com mensagem clara.

## Executar Com PostgreSQL

```bash
VP_USE_POSTGRES=true ./build-postgres/virtual_planner
```

Também configure as variáveis `POSTGRES_*` descritas em `.env.example`.

## Build Com HTTP

```bash
cmake -S back-end -B back-end/build-http -DVIRTUAL_PLANNER_WITH_HTTP=ON
cmake --build back-end/build-http --config Debug
```

`VIRTUAL_PLANNER_WITH_HTTP` é `OFF` por padrão, então o build padrão nunca
toca a rede. Esse build compila a prova de conceito do endpoint `/api/health`
(ADR-003, issue #13), que não é registrada no CTest e não roda no CI — a
única forma de reverificar o critério de aceite da issue é rodar o comando
acima manualmente.

## Docker PostgreSQL

```bash
docker compose up -d postgres
docker compose ps
```

Para parar:

```bash
docker compose stop
```

Para remover o container sem apagar o volume:

```bash
docker compose down
```

Use `docker compose down -v` apenas se quiser apagar os dados locais.

## Teste De Integração PostgreSQL

Disponível apenas no build com PostgreSQL:

```bash
ctest --test-dir build-postgres --output-on-failure -R postgres_integration_test
```

O teste usa `POSTGRES_DB`, `POSTGRES_USER` e `POSTGRES_PASSWORD`. Se essas variáveis estiverem ausentes, ele é pulado com mensagem clara.

## Smoke Test Manual Com O Compilador

Se o CMake ainda não estiver instalado, um teste isolado ainda pode ser compilado diretamente com um compilador C++20:

```bash
c++ -std=c++20 -Iinclude src/core/app_config.cpp src/shared/errors.cpp tests/unit/app_config_test.cpp -o /tmp/app_config_test
/tmp/app_config_test
```
