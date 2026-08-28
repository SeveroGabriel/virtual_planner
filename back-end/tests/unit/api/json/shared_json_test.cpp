// P-29.0: round-trip da serializacao JSON compartilhada de enums e value
// objects. Cada valor de cada enum precisa voltar identico depois de
// serializar e desserializar, senao dois donos de entidade acabariam com
// representacoes diferentes do mesmo enum.
#include "virtual_planner/api/json/shared_json.hpp"
#include "support/expect.hpp"

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>

using namespace virtual_planner;
namespace json = virtual_planner::api::json;

namespace
{

template <typename Callable>
bool throws_invalid_argument(Callable callable)
{
    try
    {
        callable();
    }
    catch (const std::invalid_argument&)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }

    return false;
}

// Serializa, confere que virou a mesma string de domain::to_string e
// desserializa de volta.
template <typename Enum, typename Parser>
void expect_enum_round_trip(Enum value, Parser parse, const std::string& label)
{
    const nlohmann::json serialized = json::to_json(value);

    VP_EXPECT(serialized.is_string(), label + " should serialize as a JSON string");
    VP_EXPECT(
        serialized.get<std::string>() == domain::to_string(value),
        label + " JSON text should match domain::to_string");
    VP_EXPECT(parse(serialized) == value, label + " should survive a JSON round-trip");
}

} // namespace

int main()
{
    // --- Enums: todos os valores de todos os enums compartilhados -----------
    for (const domain::Category value : {domain::Category::College,
                                         domain::Category::Work,
                                         domain::Category::Health,
                                         domain::Category::Leisure,
                                         domain::Category::PersonalProjects,
                                         domain::Category::Study})
    {
        expect_enum_round_trip(value, json::category_from_json, "Category");
    }

    for (const domain::GoalPeriod value : {domain::GoalPeriod::Weekly,
                                           domain::GoalPeriod::Monthly,
                                           domain::GoalPeriod::Yearly})
    {
        expect_enum_round_trip(value, json::goal_period_from_json, "GoalPeriod");
    }

    for (const domain::GoalStatus value : {domain::GoalStatus::InProgress,
                                           domain::GoalStatus::Completed,
                                           domain::GoalStatus::PartiallyCompleted,
                                           domain::GoalStatus::Failed})
    {
        expect_enum_round_trip(value, json::goal_status_from_json, "GoalStatus");
    }

    for (const domain::Priority value : {domain::Priority::Low,
                                         domain::Priority::Medium,
                                         domain::Priority::High})
    {
        expect_enum_round_trip(value, json::priority_from_json, "Priority");
    }

    for (const domain::ReminderRecurrence value : {domain::ReminderRecurrence::Once,
                                                   domain::ReminderRecurrence::Daily,
                                                   domain::ReminderRecurrence::Weekly,
                                                   domain::ReminderRecurrence::Monthly})
    {
        expect_enum_round_trip(
            value, json::reminder_recurrence_from_json, "ReminderRecurrence");
    }

    for (const domain::ReminderType value : {domain::ReminderType::Meeting,
                                             domain::ReminderType::PhoneCall,
                                             domain::ReminderType::Shopping,
                                             domain::ReminderType::Study,
                                             domain::ReminderType::Exercise,
                                             domain::ReminderType::Assignment})
    {
        expect_enum_round_trip(value, json::reminder_type_from_json, "ReminderType");
    }

    for (const domain::Shift value : {domain::Shift::Morning,
                                      domain::Shift::Afternoon,
                                      domain::Shift::Evening})
    {
        expect_enum_round_trip(value, json::shift_from_json, "Shift");
    }

    for (const domain::TaskStatus value : {domain::TaskStatus::Pending,
                                           domain::TaskStatus::Executed,
                                           domain::TaskStatus::PartiallyExecuted,
                                           domain::TaskStatus::Cancelled,
                                           domain::TaskStatus::Postponed})
    {
        expect_enum_round_trip(value, json::task_status_from_json, "TaskStatus");
    }

    // --- Enums: entradas invalidas -----------------------------------------
    VP_EXPECT(
        throws_invalid_argument([] { return json::category_from_json(nlohmann::json(42)); }),
        "a non-string JSON should be rejected as a Category");

    VP_EXPECT(
        throws_invalid_argument([] { return json::category_from_json(nlohmann::json("college")); }),
        "an unknown Category text should be rejected");

    VP_EXPECT(
        throws_invalid_argument([] { return json::task_status_from_json(nlohmann::json(nullptr)); }),
        "a null JSON should be rejected as a TaskStatus");

    // --- Date: formato ISO 8601 --------------------------------------------
    const domain::Date date{5, 3, 2026};
    const nlohmann::json serialized_date = json::to_json(date);

    VP_EXPECT(serialized_date.is_string(), "Date should serialize as a JSON string");
    VP_EXPECT(
        serialized_date.get<std::string>() == "2026-03-05",
        "Date should serialize as ISO 8601 with leading zeros");
    VP_EXPECT(
        json::date_from_json(serialized_date) == date,
        "Date should survive a JSON round-trip");

    const domain::Date leap_day{29, 2, 2024};
    VP_EXPECT(
        json::date_from_json(json::to_json(leap_day)) == leap_day,
        "a leap day should survive a JSON round-trip");

    VP_EXPECT(
        throws_invalid_argument([] { return json::date_from_json(nlohmann::json("2026-3-5")); }),
        "a date without leading zeros should be rejected");

    VP_EXPECT(
        throws_invalid_argument([] { return json::date_from_json(nlohmann::json("05/03/2026")); }),
        "the display format dd/mm/yyyy should be rejected");

    VP_EXPECT(
        throws_invalid_argument([] { return json::date_from_json(nlohmann::json("2026-02-30")); }),
        "a date that does not exist should be rejected");

    VP_EXPECT(
        throws_invalid_argument([] { return json::date_from_json(nlohmann::json("2026-1x-05")); }),
        "a non-digit in the month should be rejected");

    // --- TimeSlot: minutos de inicio e fim ---------------------------------
    const domain::TimeSlot slot{std::chrono::hours{9}, std::chrono::hours{10}};
    const nlohmann::json serialized_slot = json::to_json(slot);

    VP_EXPECT(serialized_slot.is_object(), "TimeSlot should serialize as a JSON object");
    VP_EXPECT(
        serialized_slot.at("start").get<std::int64_t>() == 540,
        "TimeSlot start should serialize as minutes from midnight");
    VP_EXPECT(
        serialized_slot.at("end").get<std::int64_t>() == 600,
        "TimeSlot end should serialize as minutes from midnight");

    const domain::TimeSlot parsed_slot = json::time_slot_from_json(serialized_slot);

    VP_EXPECT(
        parsed_slot.start() == slot.start() && parsed_slot.end() == slot.end(),
        "TimeSlot should survive a JSON round-trip");

    VP_EXPECT(
        throws_invalid_argument([] { return json::time_slot_from_json(nlohmann::json("09:00")); }),
        "a string should be rejected as a TimeSlot");

    VP_EXPECT(
        throws_invalid_argument(
            [] { return json::time_slot_from_json(nlohmann::json{{"start", 540}}); }),
        "a TimeSlot without the end field should be rejected");

    VP_EXPECT(
        throws_invalid_argument(
            [] { return json::time_slot_from_json(nlohmann::json{{"start", "540"}, {"end", 600}}); }),
        "a non-integer start should be rejected");

    VP_EXPECT(
        throws_invalid_argument(
            [] { return json::time_slot_from_json(nlohmann::json{{"start", 600}, {"end", 540}}); }),
        "an inverted interval should be rejected");

    VP_EXPECT(
        throws_invalid_argument(
            [] { return json::time_slot_from_json(nlohmann::json{{"start", 0}, {"end", 1441}}); }),
        "an interval past midnight should be rejected");

    return 0;
}
