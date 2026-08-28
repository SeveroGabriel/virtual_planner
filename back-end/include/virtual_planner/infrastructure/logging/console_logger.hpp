#pragma once

// Adapter de log para stdout/stderr (issue #71 / P-54).
//
// Sem biblioteca externa: a issue pede log minimo, e um <iostream> com
// timestamp resolve. Formato de uma linha:
//
//   2026-08-28T16:00:00Z INFO  request method=GET path=/api/health status=200
//
// Nivel abaixo de Error vai para stdout; Error vai para stderr, para que o
// operador consiga separar os dois streams.

#include "virtual_planner/interfaces/logger.hpp"

#include <mutex>

namespace virtual_planner::infrastructure::logging {

class ConsoleLogger final : public interfaces::Logger
{
public:
    explicit ConsoleLogger(interfaces::LogLevel minimum = interfaces::LogLevel::Info);

    // Le VP_LOG_LEVEL. Um valor invalido ou ausente cai em Info em vez de
    // impedir a aplicacao de subir — log e diagnostico, nao pre-requisito.
    [[nodiscard]] static interfaces::LogLevel level_from_environment();

    [[nodiscard]] interfaces::LogLevel minimum() const noexcept;

    void log(interfaces::LogLevel level,
             std::string_view message,
             std::string_view fields) override;

private:
    interfaces::LogLevel minimum_;

    // O httplib despacha handlers num thread pool, entao duas requisicoes
    // podem logar ao mesmo tempo. Sem o lock as linhas se intercalam.
    std::mutex mutex_;
};

} // namespace virtual_planner::infrastructure::logging
