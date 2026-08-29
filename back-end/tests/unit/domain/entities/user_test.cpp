#include "virtual_planner/domain/entities/user.hpp"
#include "support/expect.hpp"
#include <stdexcept>

using namespace virtual_planner::domain;

void test_user_creation_and_getters() {
    User user(1, "Gabriel", "gabriel@example.com");
    VP_EXPECT(user.id() == 1, "O ID do usuario deve ser 1");
    VP_EXPECT(user.name() == "Gabriel", "O nome do usuario deve corresponder ao valor inicial");
    VP_EXPECT(user.email() == "gabriel@example.com", "O email do usuario deve corresponder ao valor inicial");
}

void test_user_updates() {
    User user(1, "Nome Antigo", "old@example.com");
    user.update_name("Nome Atualizado");
    user.update_email("new@example.com");
    VP_EXPECT(user.name() == "Nome Atualizado", "O nome do usuario deve ser atualizado corretamente");
    VP_EXPECT(user.email() == "new@example.com", "O email do usuario deve ser atualizado corretamente");
}

void test_user_validation_rejections() {
    auto throws_invalid_argument = [](auto fn) {
        try { fn(); return false; }
        catch (const std::invalid_argument&) { return true; }
    };

    VP_EXPECT(throws_invalid_argument([](){ User user(1, "", "valido@example.com"); }),
              "Deve rejeitar a criacao de usuario com nome vazio");

    VP_EXPECT(throws_invalid_argument([](){ User user(1, "Gabriel", ""); }),
              "Deve rejeitar a criacao de usuario com email vazio");

    VP_EXPECT(throws_invalid_argument([](){ User user(1, "Gabriel", "emailinvalido.com"); }),
              "Deve rejeitar a criacao de usuario com email sem '@'");

    User valid_user(1, "Gabriel", "gabriel@example.com");

    VP_EXPECT(throws_invalid_argument([&valid_user](){ valid_user.update_name(""); }),
              "Deve rejeitar a atualizacao para um nome vazio");

    VP_EXPECT(throws_invalid_argument([&valid_user](){ valid_user.update_email(""); }),
              "Deve rejeitar a atualizacao para um email vazio");

    VP_EXPECT(throws_invalid_argument([&valid_user](){ valid_user.update_email("sem-arroba"); }),
              "Deve rejeitar a atualizacao para um email sem '@'");
}

// Regressao da issue #93: a validacao usava apenas .empty(), entao um nome ou
// email formado so de espaco em branco passava. Goal, Task e Reminder ja
// rejeitavam esse caso; User era a unica entidade que nao rejeitava.
void test_user_rejects_blank_name_and_email() {
    auto throws_invalid_argument = [](auto fn) {
        try { fn(); return false; }
        catch (const std::invalid_argument&) { return true; }
    };

    VP_EXPECT(throws_invalid_argument([](){ User user(1, "   ", "valido@example.com"); }),
              "Deve rejeitar a criacao de usuario com nome so de espacos");

    VP_EXPECT(throws_invalid_argument([](){ User user(1, "\t\n ", "valido@example.com"); }),
              "Deve rejeitar a criacao de usuario com nome so de tab e quebra de linha");

    // Um email so de espacos hoje e rejeitado pela checagem de branco, antes
    // mesmo da checagem de '@'.
    VP_EXPECT(throws_invalid_argument([](){ User user(1, "Gabriel", "   "); }),
              "Deve rejeitar a criacao de usuario com email so de espacos");

    User valid_user(1, "Gabriel", "gabriel@example.com");

    VP_EXPECT(throws_invalid_argument([&valid_user](){ valid_user.update_name("   "); }),
              "Deve rejeitar a atualizacao para um nome so de espacos");

    VP_EXPECT(throws_invalid_argument([&valid_user](){ valid_user.update_email("   "); }),
              "Deve rejeitar a atualizacao para um email so de espacos");

    // O estado nao pode ter sido corrompido por nenhuma tentativa acima.
    VP_EXPECT(valid_user.name() == "Gabriel",
              "Uma atualizacao rejeitada nao deve alterar o nome");
    VP_EXPECT(valid_user.email() == "gabriel@example.com",
              "Uma atualizacao rejeitada nao deve alterar o email");
}

void test_user_email_format_rule() {
    auto throws_invalid_argument = [](auto fn) {
        try { fn(); return false; }
        catch (const std::invalid_argument&) { return true; }
    };

    // Aceita: formato minimo local@dominio.tld.
    User accepted_simple(1, "Gabriel", "gabriel@example.com");
    VP_EXPECT(accepted_simple.email() == "gabriel@example.com",
              "Email no formato local@dominio.tld deve ser aceito");

    // Aceita: subdominio antes do ultimo ponto tambem e valido, a regra
    // so exige um '.' com texto dos dois lados na parte apos o '@'.
    User accepted_subdomain(2, "Gabriel", "gabriel@mail.example.com");
    VP_EXPECT(accepted_subdomain.email() == "gabriel@mail.example.com",
              "Email com subdominio deve ser aceito");

    // Rejeita: mais de um '@'.
    VP_EXPECT(throws_invalid_argument(
                  [](){ User user(1, "Gabriel", "a@b@example.com"); }),
              "Deve rejeitar email com mais de um '@'");

    // Rejeita: nada antes do '@'.
    VP_EXPECT(throws_invalid_argument(
                  [](){ User user(1, "Gabriel", "@example.com"); }),
              "Deve rejeitar email sem parte local, antes do '@'");

    // Rejeita: nada depois do '@'.
    VP_EXPECT(throws_invalid_argument(
                  [](){ User user(1, "Gabriel", "gabriel@"); }),
              "Deve rejeitar email sem dominio, depois do '@'");

    // Rejeita: dominio sem '.'.
    VP_EXPECT(throws_invalid_argument(
                  [](){ User user(1, "Gabriel", "gabriel@example"); }),
              "Deve rejeitar email com dominio sem '.'");

    // Rejeita: '.' logo apos o '@', sem texto antes dele no dominio.
    VP_EXPECT(throws_invalid_argument(
                  [](){ User user(1, "Gabriel", "gabriel@.com"); }),
              "Deve rejeitar email com dominio comecando em '.'");

    // Rejeita: '.' no fim do dominio, sem texto depois dele.
    VP_EXPECT(throws_invalid_argument(
                  [](){ User user(1, "Gabriel", "gabriel@example."); }),
              "Deve rejeitar email com dominio terminando em '.'");

    // A mesma regra vale para update_email, nao so para o construtor.
    User valid_user(1, "Gabriel", "gabriel@example.com");
    VP_EXPECT(throws_invalid_argument(
                  [&valid_user](){ valid_user.update_email("sem-dominio@"); }),
              "update_email deve aplicar a mesma regra de formato");
    VP_EXPECT(valid_user.email() == "gabriel@example.com",
              "Uma atualizacao de email rejeitada nao deve alterar o email");
}

int main() {
    test_user_creation_and_getters();
    test_user_updates();
    test_user_validation_rejections();
    test_user_rejects_blank_name_and_email();
    test_user_email_format_rule();

    return 0;
}
