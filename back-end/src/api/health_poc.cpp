// Prova de conceito do endpoint /api/health (issue #13 / P-27).
//
// Existe apenas para demonstrar que a biblioteca escolhida na ADR-003
// (cpp-httplib + nlohmann/json) compila e serve JSON neste projeto. Nao faz
// parte do build padrao: so e compilado com VIRTUAL_PLANNER_WITH_HTTP=ON e
// nao e registrado no CTest.
//
// Os endpoints reais, o roteamento e a composicao com os casos de uso sao
// escritos nas issues da Onda 2 (#29 / P-28 e seguintes). Nada aqui deve ser
// reaproveitado como estrutura definitiva da camada de API.
//
// Este arquivo deve ser apagado quando a issue #29 (P-28) for concluida.

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <iostream>

namespace {

constexpr char kHost[] = "0.0.0.0";
constexpr int kPort = 8080;

}  // namespace

int main() {
  httplib::Server server;

  server.Get("/api/health",
             [](const httplib::Request&, httplib::Response& response) {
               const nlohmann::json body = {{"status", "ok"}};
               response.set_content(body.dump(), "application/json");
             });

  std::cout << "PoC HTTP em http://" << kHost << ':' << kPort
            << "/api/health\n";

  if (!server.listen(kHost, kPort)) {
    std::cerr << "Falha ao abrir a porta " << kPort << ".\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
