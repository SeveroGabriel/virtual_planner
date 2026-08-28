#pragma once

// Servidor HTTP da aplicacao (issue #29 / P-28).
//
// Responsabilidades desta classe:
//
// - manter o `httplib::Server` e o ciclo de vida da porta;
// - registrar `GET /api/health`;
// - guardar as dependencias que a composition root montou, para que os donos
//   de modulo registrem os endpoints de dominio a partir delas.
//
// O que ela NAO faz: escolher implementacao concreta de repositorio ou de
// banco. Essa decisao e de `main.cpp`, que continua sendo a composition root.
//
// `httplib.h` aparece neste header de proposito: esta e a camada HTTP, e os
// donos de modulo precisam do `httplib::Server` para registrar as proprias
// rotas em arquivos separados, sem editar `api_server.cpp` — seis pessoas
// mexendo no mesmo arquivo de rotas seria conflito garantido. O que nao pode
// vazar, e nao vaza, e HTTP ou JSON dentro de `domain` e `application`.

#include <httplib.h>

#include "virtual_planner/api/http/server_config.hpp"
#include "virtual_planner/core/app_config.hpp"
#include "virtual_planner/persistence/database.hpp"
#include "virtual_planner/persistence/repository_set.hpp"

namespace virtual_planner::api::http {

class ApiServer
{
public:
    // `config` e `database` sao referenciados, nao copiados: precisam viver
    // mais que o servidor. `database` pode ser nulo — a aplicacao sobe e
    // responde sem banco nenhum, e `/api/health` reporta isso.
    ApiServer(const core::AppConfig& config,
              persistence::RepositorySet repositories,
              const persistence::Database* database);

    // Seam para os donos de modulo registrarem os endpoints de dominio.
    [[nodiscard]] httplib::Server& server() noexcept;

    [[nodiscard]] const persistence::RepositorySet& repositories() const noexcept;

    // Abre a porta sem comecar a servir. Devolve a porta efetiva — util com
    // `ServerConfig::port == 0`, que pede uma porta efemera ao sistema — ou
    // -1 quando a porta nao pode ser aberta.
    [[nodiscard]] int bind(const ServerConfig& config);

    // Serve ate `stop()`. Exige um `bind()` bem-sucedido antes. Devolve false
    // quando o laco termina por erro.
    bool serve();

    // `bind()` seguido de `serve()`. E o caminho que `main` usa.
    bool listen(const ServerConfig& config);

    // Seguro de chamar de outra thread, que e como os testes derrubam o
    // servidor.
    void stop();

private:
    void register_health_route();

    const core::AppConfig& config_;
    persistence::RepositorySet repositories_;
    const persistence::Database* database_;
    httplib::Server server_;
};

} // namespace virtual_planner::api::http
