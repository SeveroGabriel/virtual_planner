#pragma once

// Mapeamento de erro de dominio para resposta HTTP (issue #31 / P-35).
//
// Sem isto, qualquer excecao vira 500 e o frontend nao distingue "voce mandou
// um dado invalido" de "o servidor quebrou". O mapeamento vive num lugar so e
// e aplicado pelo ApiServer a TODAS as rotas, entao um dono de modulo nao
// precisa escrever try/catch no handler: basta lancar o erro certo.

#include <exception>
#include <string>

namespace virtual_planner::api::http {

struct ErrorResponse
{
    int status{500};
    // Identificador estavel para o cliente ramificar sem depender do texto.
    std::string code;
    std::string message;

    [[nodiscard]] std::string to_json() const;
};

// Mapeia:
//   shared::DomainError, std::invalid_argument -> 400 validation_error
//   shared::NotFoundError                      -> 404 not_found
//   shared::ConflictError                      -> 409 conflict
//   qualquer outra coisa                       -> 500 internal_error
//
// SEGURANCA: para 500 a mensagem original NAO entra na resposta. Um
// PersistenceError vindo do libpqxx pode carregar a connection string, e o
// ConfigError de PostgresConfig menciona variaveis de ambiente. Quem precisa
// do detalhe e o log do servidor, nao o cliente.
[[nodiscard]] ErrorResponse map_exception(std::exception_ptr error);

} // namespace virtual_planner::api::http
