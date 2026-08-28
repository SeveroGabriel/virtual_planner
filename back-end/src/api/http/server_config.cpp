#include "virtual_planner/api/http/server_config.hpp"

#include "virtual_planner/shared/errors.hpp"

#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

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

// Separa por virgula e descarta espaco em volta de cada item, para que
// "http://a, http://b" funcione tao bem quanto "http://a,http://b".
std::vector<std::string> split_origins(const std::string& value)
{
    std::vector<std::string> origins;
    std::size_t start = 0;

    while (start <= value.size())
    {
        const std::size_t comma = value.find(',', start);
        const std::size_t stop = comma == std::string::npos ? value.size() : comma;

        std::size_t first = value.find_first_not_of(" \t", start);
        std::size_t last = value.find_last_not_of(" \t", stop == 0 ? 0 : stop - 1);

        if (first != std::string::npos && first < stop && last != std::string::npos)
        {
            origins.push_back(value.substr(first, last - first + 1));
        }

        if (comma == std::string::npos)
        {
            break;
        }

        start = comma + 1;
    }

    return origins;
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

    const std::string origins =
        read_environment_or("VP_HTTP_ALLOWED_ORIGINS", std::string{});

    if (!origins.empty())
    {
        config.allowed_origins = split_origins(origins);
    }

    return config;
}

bool ServerConfig::allows_origin(std::string_view origin) const
{
    if (origin.empty())
    {
        return false;
    }

    for (const auto& allowed : allowed_origins)
    {
        if (allowed == "*" || allowed == origin)
        {
            return true;
        }
    }

    return false;
}

} // namespace virtual_planner::api::http
