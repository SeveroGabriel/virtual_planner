# Testes do modulo Task. Registre aqui qualquer teste novo de Task.
virtual_planner_add_test(
  date_test
  unit/domain/value_objects/date_test.cpp
)

virtual_planner_add_test(
  time_slot_test
  unit/domain/value_objects/time_slot_test.cpp
)

virtual_planner_add_test(
  task_status_test
  unit/domain/enums/task_status_test.cpp
)

virtual_planner_add_test(
  priority_test
  unit/domain/enums/priority_test.cpp
)

virtual_planner_add_test(
  shift_test
  unit/domain/enums/shift_test.cpp
)

virtual_planner_add_test(
  task_test
  unit/domain/entities/task_test.cpp
)
