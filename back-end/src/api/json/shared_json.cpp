#include "virtual_planner/api/json/shared_json.hpp"

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace virtual_planner::api::json {

namespace {

// Todas as falhas de desserializacao usam `std::invalid_argument`, o mesmo
// tipo que os `*_from_string` do dominio e os construtores de `Date` e
// `TimeSlot` ja lancam. Assim quem consome esta camada trata um unico tipo.

std::string read_string(const nlohmann::json& value, const char* type_name)
{
    if (!value.is_string())
    {
        throw std::invalid_argument(
            std::string{type_name} + " must be a JSON string.");
    }

    return value.get<std::string>();
}

constexpr char kIsoDateFormat[] =
    "Date must be an ISO 8601 date in the format YYYY-MM-DD.";

std::uint32_t read_iso_digits(const std::string& text,
                              std::size_t offset,
                              std::size_t length)
{
    std::uint32_t result = 0;

    for (std::size_t index = 0; index < length; ++index)
    {
        const char digit = text[offset + index];

        if (digit < '0' || digit > '9')
        {
            throw std::invalid_argument(kIsoDateFormat);
        }

        result = result * 10 + static_cast<std::uint32_t>(digit - '0');
    }

    return result;
}

domain::TimeSlot::Minutes read_minutes(const nlohmann::json& value,
                                       const char* field)
{
    if (!value.contains(field))
    {
        throw std::invalid_argument(
            std::string{"TimeSlot requires the field \""} + field + "\".");
    }

    const nlohmann::json& field_value = value.at(field);

    if (!field_value.is_number_integer())
    {
        throw std::invalid_argument(
            std::string{"TimeSlot field \""} + field
            + "\" must be an integer number of minutes.");
    }

    return domain::TimeSlot::Minutes{field_value.get<std::int64_t>()};
}

} // namespace

nlohmann::json to_json(domain::Category value)
{
    return domain::to_string(value);
}

nlohmann::json to_json(domain::GoalPeriod value)
{
    return domain::to_string(value);
}

nlohmann::json to_json(domain::GoalStatus value)
{
    return domain::to_string(value);
}

nlohmann::json to_json(domain::Priority value)
{
    return domain::to_string(value);
}

nlohmann::json to_json(domain::ReminderRecurrence value)
{
    return domain::to_string(value);
}

nlohmann::json to_json(domain::ReminderType value)
{
    return domain::to_string(value);
}

nlohmann::json to_json(domain::Shift value)
{
    return domain::to_string(value);
}

nlohmann::json to_json(domain::TaskStatus value)
{
    return domain::to_string(value);
}

domain::Category category_from_json(const nlohmann::json& value)
{
    return domain::category_from_string(read_string(value, "Category"));
}

domain::GoalPeriod goal_period_from_json(const nlohmann::json& value)
{
    return domain::goal_period_from_string(read_string(value, "GoalPeriod"));
}

domain::GoalStatus goal_status_from_json(const nlohmann::json& value)
{
    return domain::goal_status_from_string(read_string(value, "GoalStatus"));
}

domain::Priority priority_from_json(const nlohmann::json& value)
{
    return domain::priority_from_string(read_string(value, "Priority"));
}

domain::ReminderRecurrence reminder_recurrence_from_json(const nlohmann::json& value)
{
    return domain::reminder_recurrence_from_string(
        read_string(value, "ReminderRecurrence"));
}

domain::ReminderType reminder_type_from_json(const nlohmann::json& value)
{
    return domain::reminder_type_from_string(read_string(value, "ReminderType"));
}

domain::Shift shift_from_json(const nlohmann::json& value)
{
    return domain::shift_from_string(read_string(value, "Shift"));
}

domain::TaskStatus task_status_from_json(const nlohmann::json& value)
{
    return domain::task_status_from_string(read_string(value, "TaskStatus"));
}

nlohmann::json to_json(const domain::Date& value)
{
    std::ostringstream stream;

    stream << std::setfill('0')
           << std::setw(4) << value.year()
           << '-'
           << std::setw(2) << value.month()
           << '-'
           << std::setw(2) << value.day();

    return stream.str();
}

domain::Date date_from_json(const nlohmann::json& value)
{
    const std::string text = read_string(value, "Date");

    // Formato estrito: nada de "2026-8-1" nem de sufixo de hora. Aceitar
    // variacoes aqui faria cada dono de entidade emitir uma data diferente.
    if (text.size() != 10 || text[4] != '-' || text[7] != '-')
    {
        throw std::invalid_argument(kIsoDateFormat);
    }

    const std::uint32_t year = read_iso_digits(text, 0, 4);
    const std::uint32_t month = read_iso_digits(text, 5, 2);
    const std::uint32_t day = read_iso_digits(text, 8, 2);

    // O construtor valida mes, ano e dia (inclusive ano bissexto) e lanca
    // std::invalid_argument quando a data nao existe.
    return domain::Date{day, month, year};
}

nlohmann::json to_json(const domain::TimeSlot& value)
{
    return nlohmann::json{
        {"start", value.start().count()},
        {"end", value.end().count()},
    };
}

domain::TimeSlot time_slot_from_json(const nlohmann::json& value)
{
    if (!value.is_object())
    {
        throw std::invalid_argument("TimeSlot must be a JSON object.");
    }

    // O construtor valida start >= 0, end <= 24h e end > start.
    return domain::TimeSlot{
        read_minutes(value, "start"),
        read_minutes(value, "end")};
}

} // namespace virtual_planner::api::json
