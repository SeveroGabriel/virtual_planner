#include "virtual_planner/api/http/api_server.hpp"

#include <nlohmann/json.hpp>

#include <string>

namespace virtual_planner::api::http {

ApiServer::ApiServer(const core::AppConfig& config,
                     persistence::RepositorySet repositories,
                     const persistence::Database* database)
    : config_(config),
      repositories_(repositories),
      database_(database)
{
    register_health_route();
}

httplib::Server& ApiServer::server() noexcept
{
    return server_;
}

const persistence::RepositorySet& ApiServer::repositories() const noexcept
{
    return repositories_;
}

int ApiServer::bind(const ServerConfig& config)
{
    // Atencao: o segundo parametro de bind_to_any_port e socket_flags, nao a
    // porta — ele SEMPRE pede uma porta efemera. So bind_to_port aceita uma
    // porta escolhida, e devolve apenas sucesso ou falha, por isso a porta
    // efetiva vem de lugares diferentes nos dois casos.
    if (config.port == 0)
    {
        return server_.bind_to_any_port(config.host);
    }

    return server_.bind_to_port(config.host, config.port) ? config.port : -1;
}

bool ApiServer::serve()
{
    return server_.listen_after_bind();
}

bool ApiServer::listen(const ServerConfig& config)
{
    if (bind(config) < 0)
    {
        return false;
    }

    return serve();
}

void ApiServer::stop()
{
    server_.stop();
}

void ApiServer::register_health_route()
{
    server_.Get("/api/health",
                [this](const httplib::Request&, httplib::Response& response) {
                    const bool database_configured = database_ != nullptr;
                    const bool database_connected =
                        database_configured && database_->is_connected();

                    nlohmann::json body;
                    body["app"] = config_.app_name();
                    body["profile"] =
                        std::string{core::to_string(config_.profile())};
                    body["database"] = {
                        {"configured", database_configured},
                        {"connected", database_connected},
                    };

                    // Sempre 200: a resposta chegar ja prova que o processo
                    // esta de pe, que e o que um health check precisa saber.
                    // "degraded" cobre o caso de o banco estar configurado e
                    // fora do ar — informacao para quem consome, nao falha do
                    // servidor.
                    body["status"] =
                        (!database_configured || database_connected)
                            ? "ok"
                            : "degraded";

                    response.set_content(body.dump(), "application/json");
                });
}

} // namespace virtual_planner::api::http
