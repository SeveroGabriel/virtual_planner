#pragma once

// Host e porta do servidor HTTP (issue #29 / P-28).
//
// Espelha o formato de `PostgresConfig`: um struct simples com uma fabrica
// que le o ambiente, para que nenhum valor de rede fique fixo no codigo.

#include <string>

namespace virtual_planner::api::http {

struct ServerConfig
{
    // 0.0.0.0 para responder tambem de fora do container. A porta 8080 e a
    // mesma que o PoC da ADR-003 usava.
    std::string host{"0.0.0.0"};
    int port{8080};

    // Le `VP_HTTP_HOST` e `VP_HTTP_PORT`, caindo nos valores acima quando a
    // variavel nao existe ou esta vazia.
    //
    // Lanca `shared::ConfigError` quando `VP_HTTP_PORT` nao e um inteiro
    // entre 0 e 65535. A porta 0 e valida e pede uma porta efemera ao
    // sistema operacional — e assim que os testes sobem o servidor sem
    // disputar uma porta fixa.
    [[nodiscard]] static ServerConfig from_environment();
};

} // namespace virtual_planner::api::http
