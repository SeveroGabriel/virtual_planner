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

int main() {
    test_user_creation_and_getters();
    test_user_updates();
    test_user_validation_rejections();

    return 0;
}
