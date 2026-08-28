#pragma once

// Serializacao JSON compartilhada dos enums e value objects do dominio
// (issue #30 / P-29.0).
//
// Existe para que Goal, Task e Reminder usem UMA unica representacao de cada
// enum e de cada value object. Nao serialize estes tipos a mao em outro lugar:
// use as funcoes abaixo. O formato esta documentado em docs/api.md.
//
// Fica em `api` porque serializacao e detalhe de interface: o dominio nao
// conhece JSON e nao depende de nlohmann. As conversoes reaproveitam
// `domain::to_string` e os `*_from_string` de cada enum, de modo que a
// representacao textual e sempre a mesma do dominio.

#include <nlohmann/json.hpp>

#include "virtual_planner/domain/enums/category.hpp"
#include "virtual_planner/domain/enums/goal_period.hpp"
#include "virtual_planner/domain/enums/goal_status.hpp"
#include "virtual_planner/domain/enums/priority.hpp"
#include "virtual_planner/domain/enums/reminder_recurrence.hpp"
#include "virtual_planner/domain/enums/reminder_type.hpp"
#include "virtual_planner/domain/enums/shift.hpp"
#include "virtual_planner/domain/enums/task_status.hpp"
#include "virtual_planner/domain/value_objects/date.hpp"
#include "virtual_planner/domain/value_objects/time_slot.hpp"

namespace virtual_planner::api::json {

// --- Enums ------------------------------------------------------------------
//
// Serializam como string JSON com exatamente o mesmo texto de
// `domain::to_string`, em PascalCase (por exemplo "PersonalProjects").

nlohmann::json to_json(domain::Category value);
nlohmann::json to_json(domain::GoalPeriod value);
nlohmann::json to_json(domain::GoalStatus value);
nlohmann::json to_json(domain::Priority value);
nlohmann::json to_json(domain::ReminderRecurrence value);
nlohmann::json to_json(domain::ReminderType value);
nlohmann::json to_json(domain::Shift value);
nlohmann::json to_json(domain::TaskStatus value);

// Cada parser lanca `std::invalid_argument` quando o JSON nao e uma string ou
// quando o texto nao corresponde a nenhum valor do enum.

domain::Category category_from_json(const nlohmann::json& value);
domain::GoalPeriod goal_period_from_json(const nlohmann::json& value);
domain::GoalStatus goal_status_from_json(const nlohmann::json& value);
domain::Priority priority_from_json(const nlohmann::json& value);
domain::ReminderRecurrence reminder_recurrence_from_json(const nlohmann::json& value);
domain::ReminderType reminder_type_from_json(const nlohmann::json& value);
domain::Shift shift_from_json(const nlohmann::json& value);
domain::TaskStatus task_status_from_json(const nlohmann::json& value);

// --- Value objects ----------------------------------------------------------

// `Date` serializa como string ISO 8601 "YYYY-MM-DD", sempre com zero a
// esquerda. Note que NAO e o formato de `Date::to_string()`, que e
// "dd/mm/yyyy" e existe para exibicao, nao para o contrato da API.
nlohmann::json to_json(const domain::Date& value);

// Lanca `std::invalid_argument` quando o JSON nao e uma string ISO 8601 ou
// quando a data nao existe no calendario.
domain::Date date_from_json(const nlohmann::json& value);

// `TimeSlot` serializa como objeto {"start": <minutos>, "end": <minutos>},
// ambos inteiros contados a partir da meia-noite do mesmo dia.
nlohmann::json to_json(const domain::TimeSlot& value);

// Lanca `std::invalid_argument` quando o JSON nao e um objeto com os dois
// campos inteiros, ou quando o intervalo viola as invariantes de `TimeSlot`.
domain::TimeSlot time_slot_from_json(const nlohmann::json& value);

} // namespace virtual_planner::api::json
