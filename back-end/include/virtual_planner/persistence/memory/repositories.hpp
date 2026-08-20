// Inclui os quatro repositorios in-memory de uma vez.
// Espelha virtual_planner/persistence/repositories.hpp, que faz o mesmo para
// os contratos.

#pragma once

#include "virtual_planner/persistence/memory/in_memory_goal_repository.hpp"
#include "virtual_planner/persistence/memory/in_memory_task_repository.hpp"
#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"
#include "virtual_planner/persistence/memory/in_memory_user_repository.hpp"
