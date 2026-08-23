// P-15.3: round-trip e caminho de erro de virtual_planner::domain::Priority.
#include "virtual_planner/domain/enums/priority.hpp"
#include "support/expect.hpp"

#include <stdexcept>
#include <string>

using namespace virtual_planner;

int main()
{
    // --- Round-trip: to_string(from_string(x)) == x para todo valor -----------
    VP_EXPECT(domain::to_string(domain::Priority::Low) == "Low", "Low should serialize as \"Low\"");
    VP_EXPECT(domain::to_string(domain::Priority::Medium) == "Medium", "Medium should serialize as \"Medium\"");
    VP_EXPECT(domain::to_string(domain::Priority::High) == "High", "High should serialize as \"High\"");

    VP_EXPECT(
        domain::priority_from_string("Low") == domain::Priority::Low,
        "\"Low\" should parse back to Priority::Low"
    );
    VP_EXPECT(
        domain::priority_from_string("Medium") == domain::Priority::Medium,
        "\"Medium\" should parse back to Priority::Medium"
    );
    VP_EXPECT(
        domain::priority_from_string("High") == domain::Priority::High,
        "\"High\" should parse back to Priority::High"
    );

    // --- Caminho de erro: valor invalido deve lançar com mensagem correta -----
    try
    {
        domain::priority_from_string("Invalid");
        VP_EXPECT(false, "priority_from_string should throw for an unknown value");
    }
    catch (const std::invalid_argument& error)
    {
        VP_EXPECT(
            std::string(error.what()) == "Invalid Priority",
            "priority_from_string should report its own enum name"
        );
    }

    return 0;
}
