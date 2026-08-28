#include "virtual_planner/api/http/error_response.hpp"

#include "virtual_planner/shared/errors.hpp"

#include <nlohmann/json.hpp>

#include <stdexcept>

namespace virtual_planner::api::http {

namespace {

constexpr char kInternalMessage[] =
    "Erro interno do servidor. Consulte o log do servidor para o detalhe.";

} // namespace

std::string ErrorResponse::to_json() const
{
    const nlohmann::json body = {
        {"error",
         {
             {"code", code},
             {"message", message},
         }},
    };

    return body.dump();
}

ErrorResponse map_exception(std::exception_ptr error)
{
    if (!error)
    {
        return ErrorResponse{500, "internal_error", kInternalMessage};
    }

    try
    {
        std::rethrow_exception(error);
    }
    catch (const shared::NotFoundError& not_found)
    {
        return ErrorResponse{404, "not_found", not_found.what()};
    }
    catch (const shared::ConflictError& conflict)
    {
        return ErrorResponse{409, "conflict", conflict.what()};
    }
    catch (const shared::DomainError& domain_error)
    {
        return ErrorResponse{400, "validation_error", domain_error.what()};
    }
    catch (const std::invalid_argument& invalid)
    {
        // Cobre os value objects (Date, TimeSlot), os *_from_string dos enums
        // e os parsers de api::json, que lancam invalid_argument. Todos sao
        // entrada malformada do cliente, nao falha do servidor.
        return ErrorResponse{400, "validation_error", invalid.what()};
    }
    catch (...)
    {
        // Inclui PersistenceError, ConfigError e qualquer excecao inesperada.
        // A mensagem original fica de fora de proposito: ver o comentario de
        // seguranca no header.
        return ErrorResponse{500, "internal_error", kInternalMessage};
    }
}

} // namespace virtual_planner::api::http
