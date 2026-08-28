#include "virtual_planner/api/http/server_config.hpp"

#include "virtual_planner/shared/errors.hpp"

#include <cstddef>
#include <cstdlib>
#include <string>

namespace virtual_planner::api::http {

namespace {

// Devolve o fallback tambem quando a variavel existe mas esta vazia: um
// `VP_HTTP_HOST=` no .env nao deve virar um host vazio.
std::string read_environment_or(const char* name, std::string fallback)
{
    const char* value = std::getenv(name);

    if (value == nullptr || *value == '\0')
    {
        return fallback;
    }

    return value;
}

} // namespace

ServerConfig ServerConfig::from_environment()
{
    ServerConfig config;
    config.host = read_environment_or("VP_HTTP_HOST", config.host);

    const std::string port_text =
        read_environment_or("VP_HTTP_PORT", std::to_string(config.port));

    // std::stoi aceitaria "8080abc" e pararia no primeiro caractere invalido.
    // Uma porta so parcialmente lida e pior que um erro: o servidor subiria
    // em um lugar que ninguem pediu.
    std::size_t consumed = 0;
    int port = 0;

    try
    {
        port = std::stoi(port_text, &consumed);
    }
    catch (const std::exception&)
    {
        throw shared::ConfigError(
            "VP_HTTP_PORT must be an integer between 0 and 65535, got \""
            + port_text + "\".");
    }

    if (consumed != port_text.size() || port < 0 || port > 65535)
    {
        throw shared::ConfigError(
            "VP_HTTP_PORT must be an integer between 0 and 65535, got \""
            + port_text + "\".");
    }

    config.port = port;
    return config;
}

} // namespace virtual_planner::api::http
