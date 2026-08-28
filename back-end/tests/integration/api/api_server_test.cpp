// P-28: teste de integracao do servidor HTTP.
//
// Sobe o servidor de verdade em uma porta efemera de 127.0.0.1 e fala com ele
// por um cliente HTTP. Testar o handler de /api/health direto, sem servidor,
// nao provaria o criterio de aceite: o que precisa ficar demonstrado e que a
// aplicacao sobe e responde, com e sem banco.
#include "virtual_planner/api/http/api_server.hpp"
#include "virtual_planner/api/http/server_config.hpp"
#include "virtual_planner/persistence/memory/repositories.hpp"
#include "virtual_planner/persistence/repository_set.hpp"
#include "virtual_planner/shared/errors.hpp"
#include "support/expect.hpp"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <string>
#include <thread>

using namespace virtual_planner;
namespace http_api = virtual_planner::api::http;

namespace
{

// Database concreta sem banco nenhum atras: os hooks virtuais de
// persistence::Database ja tem implementacao vazia, entao connect() apenas
// leva o estado para Connected. Serve para exercitar o relato de /api/health
// sem exigir PostgreSQL no CI.
class FakeDatabase final : public persistence::Database
{
};

template <typename Callable>
void with_running_server(http_api::ApiServer& server, Callable callable)
{
    http_api::ServerConfig config;
    config.host = "127.0.0.1";
    // Porta 0: o sistema escolhe uma porta livre. Fixar uma porta faria o
    // teste falhar de forma intermitente sempre que algo ja estivesse nela.
    config.port = 0;

    const int port = server.bind(config);
    VP_EXPECT(port > 0, "server should bind an ephemeral port");

    std::thread serving([&server] { server.serve(); });
    server.server().wait_until_ready();

    httplib::Client client("127.0.0.1", port);
    client.set_read_timeout(5, 0);

    callable(client);

    server.stop();
    serving.join();
}

persistence::RepositorySet make_repositories(
    persistence::InMemoryGoalRepository& goals,
    persistence::InMemoryTaskRepository& tasks,
    persistence::InMemoryReminderRepository& reminders,
    persistence::InMemoryUserRepository& users)
{
    return persistence::RepositorySet{&goals, &tasks, &reminders, &users};
}

bool throws_config_error(const char* port_value)
{
    if (port_value == nullptr)
    {
        ::unsetenv("VP_HTTP_PORT");
    }
    else
    {
        ::setenv("VP_HTTP_PORT", port_value, 1);
    }

    try
    {
        static_cast<void>(http_api::ServerConfig::from_environment());
    }
    catch (const shared::ConfigError&)
    {
        ::unsetenv("VP_HTTP_PORT");
        return true;
    }

    ::unsetenv("VP_HTTP_PORT");
    return false;
}

} // namespace

int main()
{
    persistence::InMemoryGoalRepository goals;
    persistence::InMemoryTaskRepository tasks;
    persistence::InMemoryReminderRepository reminders;
    persistence::InMemoryUserRepository users;

    const auto repositories = make_repositories(goals, tasks, reminders, users);
    const core::AppConfig config{"virtual-planner-test",
                                 core::ExecutionProfile::Test};

    // --- Sem banco: a aplicacao sobe e responde mesmo assim ----------------
    {
        http_api::ApiServer server{config, repositories, nullptr};

        with_running_server(server, [](httplib::Client& client) {
            const auto response = client.Get("/api/health");

            VP_EXPECT(static_cast<bool>(response), "GET /api/health should answer");
            VP_EXPECT(response->status == 200, "/api/health should answer 200");
            VP_EXPECT(
                response->get_header_value("Content-Type") == "application/json",
                "/api/health should answer application/json");

            const auto body = nlohmann::json::parse(response->body);

            VP_EXPECT(body.at("app") == "virtual-planner-test",
                      "/api/health should report the configured app name");
            VP_EXPECT(body.at("profile") == "test",
                      "/api/health should report the execution profile");
            VP_EXPECT(body.at("status") == "ok",
                      "without a database the status should be ok");
            VP_EXPECT(body.at("database").at("configured") == false,
                      "without a database it should not be reported as configured");
            VP_EXPECT(body.at("database").at("connected") == false,
                      "without a database it should not be reported as connected");
        });
    }

    // --- Com banco conectado -----------------------------------------------
    {
        FakeDatabase database;
        database.connect();
        VP_EXPECT(database.is_connected(), "the fake database should be connected");

        http_api::ApiServer server{config, repositories, &database};

        with_running_server(server, [](httplib::Client& client) {
            const auto response = client.Get("/api/health");

            VP_EXPECT(static_cast<bool>(response), "GET /api/health should answer");
            VP_EXPECT(response->status == 200, "/api/health should answer 200");

            const auto body = nlohmann::json::parse(response->body);

            VP_EXPECT(body.at("status") == "ok",
                      "a connected database should keep the status ok");
            VP_EXPECT(body.at("database").at("configured") == true,
                      "a wired database should be reported as configured");
            VP_EXPECT(body.at("database").at("connected") == true,
                      "a connected database should be reported as connected");
        });
    }

    // --- Com banco configurado mas fora do ar ------------------------------
    {
        // Construida e nunca conectada: e o cenario de banco caido.
        FakeDatabase database;
        VP_EXPECT(!database.is_connected(),
                  "a database that never connected should not be connected");

        http_api::ApiServer server{config, repositories, &database};

        with_running_server(server, [](httplib::Client& client) {
            const auto response = client.Get("/api/health");

            VP_EXPECT(static_cast<bool>(response), "GET /api/health should answer");
            VP_EXPECT(response->status == 200,
                      "a database that is down should still answer 200");

            const auto body = nlohmann::json::parse(response->body);

            VP_EXPECT(body.at("status") == "degraded",
                      "a configured database that is down should degrade the status");
            VP_EXPECT(body.at("database").at("configured") == true,
                      "the database should still be reported as configured");
            VP_EXPECT(body.at("database").at("connected") == false,
                      "a database that is down should not be reported as connected");
        });
    }

    // --- Rota desconhecida --------------------------------------------------
    {
        http_api::ApiServer server{config, repositories, nullptr};

        with_running_server(server, [](httplib::Client& client) {
            const auto response = client.Get("/api/does-not-exist");

            VP_EXPECT(static_cast<bool>(response), "an unknown route should answer");
            VP_EXPECT(response->status == 404, "an unknown route should answer 404");
        });
    }

    // --- Repositorios entregues pela composition root ----------------------
    {
        http_api::ApiServer server{config, repositories, nullptr};

        VP_EXPECT(server.repositories().goals == &goals,
                  "the server should keep the goal repository it was given");
        VP_EXPECT(server.repositories().tasks == &tasks,
                  "the server should keep the task repository it was given");
        VP_EXPECT(server.repositories().reminders == &reminders,
                  "the server should keep the reminder repository it was given");
        VP_EXPECT(server.repositories().users == &users,
                  "the server should keep the user repository it was given");
    }

    // --- Porta explicita ----------------------------------------------------
    //
    // Regressao: bind_to_any_port(host, flags) recebe socket_flags no segundo
    // parametro, nao a porta, e sempre abre uma porta efemera. Passar a porta
    // ali fazia o servidor subir em uma porta aleatoria enquanto o log dizia
    // outra. Os cenarios acima usam porta 0 e nao pegariam isso.
    {
        // Descobre uma porta livre deixando o sistema escolher uma e
        // devolvendo-a em seguida.
        //
        // O stop() aqui NAO e opcional. httplib::Server tem destrutor
        // defaulted: sair de escopo nao fecha o socket de escuta, e stop() e a
        // unica coisa que libera o descritor — o comentario dele no httplib.h
        // diz exatamente isso. Sem stop(), o probe vira um listener orfao na
        // porta, sem ninguem chamando accept.
        //
        // E ai o detalhe que transforma o vazamento em teste intermitente:
        // httplib liga SO_REUSEPORT por padrao, nao SO_REUSEADDR. Com
        // SO_REUSEPORT o servidor real consegue abrir a MESMA porta, os dois
        // sockets ficam escutando, e o kernel distribui as conexoes entre
        // eles. As que caem no orfao nunca sao aceitas e morrem no timeout de
        // leitura. Foi assim que este teste falhou no CI enquanto passava na
        // maquina local.
        int free_port = 0;
        {
            httplib::Server probe;
            free_port = probe.bind_to_any_port("127.0.0.1");
            VP_EXPECT(free_port > 0, "the probe should find a free port");
            probe.stop();
        }

        // Guarda do proprio vazamento descrito acima: se o probe tivesse
        // deixado o socket aberto, alguem ainda estaria escutando nesta porta
        // agora, e o connect passaria em vez de ser recusado. Sem esta
        // assercao, remover o stop() acima volta a produzir uma falha
        // intermitente e dificil de rastrear em vez de um erro direto.
        {
            httplib::Client leak_check("127.0.0.1", free_port);
            leak_check.set_connection_timeout(1, 0);

            VP_EXPECT(
                leak_check.Get("/api/health").error() == httplib::Error::Connection,
                "the probe must release the port before the server binds it");
        }

        http_api::ServerConfig config_with_port;
        config_with_port.host = "127.0.0.1";
        config_with_port.port = free_port;

        http_api::ApiServer server{config, repositories, nullptr};

        const int bound = server.bind(config_with_port);
        VP_EXPECT(bound == free_port,
                  "bind should return exactly the port that was asked for");

        std::thread serving([&server] { server.serve(); });
        server.server().wait_until_ready();

        httplib::Client client("127.0.0.1", free_port);
        client.set_read_timeout(5, 0);

        const auto response = client.Get("/api/health");

        server.stop();
        serving.join();

        VP_EXPECT(static_cast<bool>(response),
                  "the server should answer on the port that was asked for");
        VP_EXPECT(response->status == 200,
                  "/api/health should answer 200 on an explicit port");
    }

    // --- ServerConfig::from_environment ------------------------------------
    ::unsetenv("VP_HTTP_HOST");
    ::unsetenv("VP_HTTP_PORT");

    {
        const auto defaults = http_api::ServerConfig::from_environment();
        VP_EXPECT(defaults.host == "0.0.0.0", "the default host should be 0.0.0.0");
        VP_EXPECT(defaults.port == 8080, "the default port should be 8080");
    }

    {
        ::setenv("VP_HTTP_HOST", "127.0.0.1", 1);
        ::setenv("VP_HTTP_PORT", "9090", 1);

        const auto from_environment = http_api::ServerConfig::from_environment();
        VP_EXPECT(from_environment.host == "127.0.0.1",
                  "VP_HTTP_HOST should override the default host");
        VP_EXPECT(from_environment.port == 9090,
                  "VP_HTTP_PORT should override the default port");

        ::unsetenv("VP_HTTP_HOST");
        ::unsetenv("VP_HTTP_PORT");
    }

    {
        // Variavel presente e vazia deve cair no padrao, nao virar host vazio.
        ::setenv("VP_HTTP_HOST", "", 1);
        const auto empty_host = http_api::ServerConfig::from_environment();
        VP_EXPECT(empty_host.host == "0.0.0.0",
                  "an empty VP_HTTP_HOST should fall back to the default");
        ::unsetenv("VP_HTTP_HOST");
    }

    VP_EXPECT(throws_config_error("not-a-number"),
              "a non-numeric VP_HTTP_PORT should be rejected");
    VP_EXPECT(throws_config_error("8080abc"),
              "a partially numeric VP_HTTP_PORT should be rejected");
    VP_EXPECT(throws_config_error("70000"),
              "a VP_HTTP_PORT above 65535 should be rejected");
    VP_EXPECT(throws_config_error("-1"),
              "a negative VP_HTTP_PORT should be rejected");
    VP_EXPECT(!throws_config_error("0"),
              "VP_HTTP_PORT 0 is valid and means an ephemeral port");

    return 0;
}
