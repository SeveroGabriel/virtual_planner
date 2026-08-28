# Fontes do dominio: predicados compartilhados, enums, value objects e entidades.
virtual_planner_add_sources(
  domain/text.cpp

  domain/enums/category.cpp
  domain/enums/priority.cpp
  domain/enums/task_status.cpp
  domain/enums/goal_status.cpp
  domain/enums/goal_period.cpp
  domain/enums/reminder_recurrence.cpp
  domain/enums/reminder_type.cpp
  domain/enums/shift.cpp

  domain/value_objects/time_slot.cpp
  domain/value_objects/date.cpp

  domain/entities/user.cpp
  domain/entities/reminder.cpp
  domain/entities/task.cpp
  domain/entities/goal.cpp
)
