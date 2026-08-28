#pragma once

// Porta de log da aplicacao (issue #71 / P-54).
//
// Fica em `interfaces` para que `application` e `api` possam registrar
// eventos sem conhecer para onde o log vai. O adapter concreto vive em
// `infrastructure/logging`.

#include <optional>
#include <string>
#include <string_view>

namespace virtual_planner::interfaces {

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
};

std::string_view to_string(LogLevel level);

// nullopt quando o texto nao corresponde a nenhum nivel. Diferente dos
// enums de dominio, aqui nao se lanca: um VP_LOG_LEVEL escrito errado nao
// deve impedir a aplicacao de subir, so cair no nivel padrao.
std::optional<LogLevel> log_level_from_string(std::string_view value);

class Logger
{
public:
    virtual ~Logger() = default;

    // `fields` e um texto ja formatado como chave=valor, anexado depois da
    // mensagem. Cabe a quem chama garantir que nada sensivel entre aqui:
    // este projeto nao loga corpo de requisicao, header nem connection
    // string em lugar nenhum.
    virtual void log(LogLevel level,
                     std::string_view message,
                     std::string_view fields) = 0;

    void debug(std::string_view message, std::string_view fields = {});
    void info(std::string_view message, std::string_view fields = {});
    void warning(std::string_view message, std::string_view fields = {});
    void error(std::string_view message, std::string_view fields = {});
};

} // namespace virtual_planner::interfaces
