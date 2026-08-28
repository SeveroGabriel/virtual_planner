#pragma once

// Host e porta do servidor HTTP (issue #29 / P-28).
//
// Espelha o formato de `PostgresConfig`: um struct simples com uma fabrica
// que le o ambiente, para que nenhum valor de rede fique fixo no codigo.

#include <string>
#include <string_view>
#include <vector>

namespace virtual_planner::api::http {

struct ServerConfig
{
    // 0.0.0.0 para responder tambem de fora do container. A porta 8080 e a
    // mesma que o PoC da ADR-003 usava.
    std::string host{"0.0.0.0"};
    int port{8080};

    // Origens autorizadas a chamar a API pelo navegador (issue #32).
    //
    // O padrao e o servidor de desenvolvimento do Vite, que e de onde o
    // frontend chama a API hoje. Uma lista com um unico "*" libera qualquer
    // origem — util para experimentar, ruim para producao.
    std::vector<std::string> allowed_origins{"http://localhost:5173"};

    // Le `VP_HTTP_HOST`, `VP_HTTP_PORT` e `VP_HTTP_ALLOWED_ORIGINS`, caindo
    // nos valores acima quando a variavel nao existe ou esta vazia.
    //
    // `VP_HTTP_ALLOWED_ORIGINS` e uma lista separada por virgula; espacos ao
    // redor de cada item sao ignorados.
    //
    // Lanca `shared::ConfigError` quando `VP_HTTP_PORT` nao e um inteiro
    // entre 0 e 65535. A porta 0 e valida e pede uma porta efemera ao
    // sistema operacional — e assim que os testes sobem o servidor sem
    // disputar uma porta fixa.
    [[nodiscard]] static ServerConfig from_environment();

    // True quando a origem recebida pode receber os cabecalhos de CORS.
    // Uma origem vazia (requisicao que nao veio de navegador) e sempre falsa:
    // sem `Origin` nao ha o que autorizar.
    [[nodiscard]] bool allows_origin(std::string_view origin) const;
};

} // namespace virtual_planner::api::http
