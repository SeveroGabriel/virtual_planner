# Testes dos repositorios in-memory. Registre aqui qualquer teste novo de
# persistence/memory.

virtual_planner_add_test(
  in_memory_goal_repository_test
  unit/persistence/in_memory_goal_repository_test.cpp
)

virtual_planner_add_test(
  in_memory_task_repository_test
  unit/persistence/in_memory_task_repository_test.cpp
)

virtual_planner_add_test(
  in_memory_reminder_repository_test
  unit/persistence/in_memory_reminder_repository_test.cpp
)

virtual_planner_add_test(
  in_memory_user_repository_test
  unit/persistence/in_memory_user_repository_test.cpp
)
