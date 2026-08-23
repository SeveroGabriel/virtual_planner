// P-15.3: round-trip e caminho de erro de virtual_planner::domain::Shift.
#include "virtual_planner/domain/enums/shift.hpp"
#include "support/expect.hpp"

#include <stdexcept>
#include <string>

using namespace virtual_planner;

int main()
{
    // --- Round-trip: to_string(from_string(x)) == x para todo valor -----------
    VP_EXPECT(domain::to_string(domain::Shift::Morning) == "Morning", "Morning should serialize as \"Morning\"");
    VP_EXPECT(domain::to_string(domain::Shift::Afternoon) == "Afternoon", "Afternoon should serialize as \"Afternoon\"");
    VP_EXPECT(domain::to_string(domain::Shift::Evening) == "Evening", "Evening should serialize as \"Evening\"");

    VP_EXPECT(
        domain::shift_from_string("Morning") == domain::Shift::Morning,
        "\"Morning\" should parse back to Shift::Morning"
    );
    VP_EXPECT(
        domain::shift_from_string("Afternoon") == domain::Shift::Afternoon,
        "\"Afternoon\" should parse back to Shift::Afternoon"
    );
    VP_EXPECT(
        domain::shift_from_string("Evening") == domain::Shift::Evening,
        "\"Evening\" should parse back to Shift::Evening"
    );

    // --- Caminho de erro: valor invalido deve lançar com mensagem correta -----
    try
    {
        domain::shift_from_string("Invalid");
        VP_EXPECT(false, "shift_from_string should throw for an unknown value");
    }
    catch (const std::invalid_argument& error)
    {
        VP_EXPECT(
            std::string(error.what()) == "Invalid Shift",
            "shift_from_string should report its own enum name, not another enum's"
        );
    }

    return 0;
}
