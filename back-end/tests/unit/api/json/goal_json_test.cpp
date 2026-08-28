#include <nlohmann/json.hpp>

#include "virtual_planner/api/json/goal_json.hpp"
#include "support/expect.hpp"

#include <stdexcept>

using namespace virtual_planner;

namespace {

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

} // namespace

int main()
{
    // Arrange
    const domain::Goal original{
        42,
        "Finish C++ Planner",
        domain::Category::Study,
        domain::GoalStatus::InProgress,
        domain::GoalPeriod::Weekly,
        domain::Date{28, 8, 2026},
    };

    // Act
    const nlohmann::json serialized = api::json::to_json(original);

    // Assert
    VP_EXPECT(serialized.at("id") == 42,
              "Goal id should serialize correctly");
    VP_EXPECT(serialized.at("description") == "Finish C++ Planner",
              "Goal description should serialize correctly");
    VP_EXPECT(serialized.at("category") == "Study",
              "Goal category should reuse shared JSON representation");
    VP_EXPECT(serialized.at("status") == "In Progress",
              "Goal status should reuse shared JSON representation");
    VP_EXPECT(serialized.at("period") == "Weekly",
              "Goal period should reuse shared JSON representation");
    VP_EXPECT(serialized.at("reference_date") == "2026-08-28",
              "Goal reference date should reuse shared JSON representation");

    // Act
    const domain::Goal parsed = api::json::goal_from_json(serialized);

    // Assert
    VP_EXPECT(parsed.id() == original.id(),
              "Goal id should survive JSON round trip");
    VP_EXPECT(parsed.description() == original.description(),
              "Goal description should survive JSON round trip");
    VP_EXPECT(parsed.category() == original.category(),
              "Goal category should survive JSON round trip");
    VP_EXPECT(parsed.status() == original.status(),
              "Goal status should survive JSON round trip");
    VP_EXPECT(parsed.period() == original.period(),
              "Goal period should survive JSON round trip");
    VP_EXPECT(parsed.reference_date() == original.reference_date(),
              "Goal reference date should survive JSON round trip");

    // Arrange
    nlohmann::json fractional_id = serialized;
    fractional_id["id"] = 1.5;
    nlohmann::json negative_id = serialized;
    negative_id["id"] = -1;
    nlohmann::json string_id = serialized;
    string_id["id"] = "42";
    nlohmann::json missing_period = serialized;
    missing_period.erase("period");

    // Act and Assert
    VP_EXPECT(
        throws_invalid_argument(
            [&fractional_id] { return api::json::goal_from_json(fractional_id); }),
        "a fractional Goal id should be rejected");
    VP_EXPECT(
        throws_invalid_argument(
            [&negative_id] { return api::json::goal_from_json(negative_id); }),
        "a negative Goal id should be rejected");
    VP_EXPECT(
        throws_invalid_argument(
            [&string_id] { return api::json::goal_from_json(string_id); }),
        "a string Goal id should be rejected");
    VP_EXPECT(
        throws_invalid_argument(
            [] { return api::json::goal_from_json(nlohmann::json::array()); }),
        "a non-object Goal should be rejected");
    VP_EXPECT(
        throws_invalid_argument(
            [&missing_period] { return api::json::goal_from_json(missing_period); }),
        "a Goal without a required field should be rejected");

    return 0;
}
