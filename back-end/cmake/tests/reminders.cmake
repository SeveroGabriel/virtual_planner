# Testes do modulo Reminder. Registre aqui qualquer teste novo de Reminder.

virtual_planner_add_test(
  reminder_test
  unit/domain/entities/reminder_test.cpp
)

virtual_planner_add_test(
  create_reminder_use_case_test
  unit/application/reminder/create_reminder_use_case_test.cpp
)

virtual_planner_add_test(
  update_reminder_use_case_test
  unit/application/reminder/update_reminder_use_case_test.cpp
)

virtual_planner_add_test(
  delete_reminder_use_case_test
  unit/application/reminder/delete_reminder_use_case_test.cpp
)

virtual_planner_add_test(
  list_reminders_use_case_test
  unit/application/reminder/list_reminders_use_case_test.cpp
)