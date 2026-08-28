#pragma once

// Conjunto de portas de persistencia montado pela composition root
// (issue #29 / P-28).
//
// `main` decide qual implementacao concreta entra em cada campo — in-memory
// ou PostgreSQL, conforme a configuracao — e entrega o conjunto pronto para
// quem precisa dele. Assim a escolha do adapter fica em um lugar so.
//
// Os ponteiros sao NAO-DONOS: quem constroi e destroi os repositorios e a
// composition root, e eles precisam viver mais que qualquer consumidor deste
// struct. Um campo nulo significa "esta porta ainda nao foi ligada".
//
// Fica em `persistence` porque so menciona contratos: nao depende de HTTP,
// de JSON nem de PostgreSQL.

#include "virtual_planner/persistence/goal_repository.hpp"
#include "virtual_planner/persistence/reminder_repository.hpp"
#include "virtual_planner/persistence/task_repository.hpp"
#include "virtual_planner/persistence/user_repository.hpp"

namespace virtual_planner::persistence {

struct RepositorySet
{
    GoalRepository* goals{nullptr};
    TaskRepository* tasks{nullptr};
    ReminderRepository* reminders{nullptr};
    UserRepository* users{nullptr};
};

} // namespace virtual_planner::persistence
