// P-15.3: round-trip e caminho de erro de virtual_planner::domain::TaskStatus.
#include "virtual_planner/domain/enums/task_status.hpp"
#include "support/expect.hpp"

#include <stdexcept>
#include <string>

using namespace virtual_planner;

int main()
{
    // --- Round-trip: to_string(from_string(x)) == x para todo valor -----------
    VP_EXPECT(domain::to_string(domain::TaskStatus::Pending) == "Pending", "Pending should serialize as \"Pending\"");
    VP_EXPECT(domain::to_string(domain::TaskStatus::Executed) == "Executed", "Executed should serialize as \"Executed\"");
    VP_EXPECT(
        domain::to_string(domain::TaskStatus::PartiallyExecuted) == "PartiallyExecuted",
        "PartiallyExecuted should serialize as \"PartiallyExecuted\""
    );
    VP_EXPECT(domain::to_string(domain::TaskStatus::Cancelled) == "Cancelled", "Cancelled should serialize as \"Cancelled\"");
    VP_EXPECT(domain::to_string(domain::TaskStatus::Postponed) == "Postponed", "Postponed should serialize as \"Postponed\"");

    VP_EXPECT(
        domain::task_status_from_string("Pending") == domain::TaskStatus::Pending,
        "\"Pending\" should parse back to TaskStatus::Pending"
    );
    VP_EXPECT(
        domain::task_status_from_string("Executed") == domain::TaskStatus::Executed,
        "\"Executed\" should parse back to TaskStatus::Executed"
    );
    VP_EXPECT(
        domain::task_status_from_string("PartiallyExecuted") == domain::TaskStatus::PartiallyExecuted,
        "\"PartiallyExecuted\" should parse back to TaskStatus::PartiallyExecuted"
    );
    VP_EXPECT(
        domain::task_status_from_string("Cancelled") == domain::TaskStatus::Cancelled,
        "\"Cancelled\" should parse back to TaskStatus::Cancelled"
    );
    VP_EXPECT(
        domain::task_status_from_string("Postponed") == domain::TaskStatus::Postponed,
        "\"Postponed\" should parse back to TaskStatus::Postponed"
    );

    // --- Caminho de erro: valor invalido deve lançar com mensagem correta -----
    try
    {
        domain::task_status_from_string("Invalid");
        VP_EXPECT(false, "task_status_from_string should throw for an unknown value");
    }
    catch (const std::invalid_argument& error)
    {
        VP_EXPECT(
            std::string(error.what()) == "Invalid TaskStatus.",
            "task_status_from_string should report its own enum name"
        );
    }

    return 0;
}
