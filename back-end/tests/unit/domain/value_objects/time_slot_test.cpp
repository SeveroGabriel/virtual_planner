// P-15.2: cobertura de testes para virtual_planner::domain::TimeSlot,
// incluindo TimeSlot::overlaps (base de P-24 e P-41).
#include "virtual_planner/domain/value_objects/time_slot.hpp"
#include "support/expect.hpp"

#include <chrono>
#include <stdexcept>

using namespace virtual_planner;
using Minutes = domain::TimeSlot::Minutes;

namespace
{

bool throws_invalid_argument(Minutes start, Minutes end)
{
    try
    {
        domain::TimeSlot invalid{start, end};
    }
    catch (const std::invalid_argument&)
    {
        return true;
    }

    return false;
}

} // namespace

int main()
{
    //slot valido generico 
    const domain::TimeSlot slot{std::chrono::hours{9}, std::chrono::hours{10}};

    VP_EXPECT(slot.start() == std::chrono::hours{9}, "start should match constructor value");
    VP_EXPECT(slot.end() == std::chrono::hours{10}, "end should match constructor value");
    VP_EXPECT(slot.duration() == std::chrono::hours{1}, "duration should be end - start");

    //limites do dia: 00:00 e 24:00 sao aceitos como bordas
    const domain::TimeSlot full_day{Minutes{0}, std::chrono::hours{24}};
    VP_EXPECT(full_day.start() == Minutes{0}, "start at 00:00 should be accepted");
    VP_EXPECT(full_day.end() == std::chrono::hours{24}, "end at 24:00 should be accepted");

    //Cada throw de time_slot.cpp precisa de ao menos um caso

    //construcao invalida: inicio >= fim.
    VP_EXPECT(
        throws_invalid_argument(std::chrono::hours{10}, std::chrono::hours{10}),
        "start == end should be rejected"
    );
    VP_EXPECT(
        throws_invalid_argument(std::chrono::hours{10}, std::chrono::hours{9}),
        "start > end should be rejected"
    );

    //construcao invalida: valores fora do dia
    VP_EXPECT(
        throws_invalid_argument(Minutes{-1}, std::chrono::hours{1}),
        "a negative start should be rejected"
    );
    VP_EXPECT(
        throws_invalid_argument(std::chrono::hours{23}, std::chrono::hours{25}),
        "an end past 24:00 should be rejected"
    );

    // overlaps() usa comparacoes estritas (start < other.end && end > other.start),
    // entao dois slots que apenas se tocam (fim de um == inicio do outro) sao
    // considerados ADJACENTES, nao sobrepostos. Essa semantica e travada aqui
    // porque P-24 (deteccao de conflito) depende dela: um slot terminando as
    // 10:00 nao conflita com um slot comecando as 10:00.

    const domain::TimeSlot base{std::chrono::hours{9}, std::chrono::hours{11}};

    //sobreposicao parcial: [9,11) e [10,12) se cruzam sem um conter o outro
    const domain::TimeSlot partial{std::chrono::hours{10}, std::chrono::hours{12}};
    VP_EXPECT(base.overlaps(partial), "partially overlapping slots should overlap");
    VP_EXPECT(partial.overlaps(base), "overlaps should be symmetric for partial overlap");

    //sobreposicao contida: [9,11) contem [9:30,10:30).
    const domain::TimeSlot contained{
        std::chrono::hours{9} + std::chrono::minutes{30},
        std::chrono::hours{10} + std::chrono::minutes{30}
    };
    VP_EXPECT(base.overlaps(contained), "a slot fully containing another should overlap");
    VP_EXPECT(contained.overlaps(base), "overlaps should be symmetric for containment");

    //sobreposicao identica: [9,11) e [9,11).
    const domain::TimeSlot identical{std::chrono::hours{9}, std::chrono::hours{11}};
    VP_EXPECT(base.overlaps(identical), "identical slots should overlap");

    //[9,11) e [13,14) nao tem nenhum ponto em comum.
    const domain::TimeSlot disjoint{std::chrono::hours{13}, std::chrono::hours{14}};
    VP_EXPECT(!base.overlaps(disjoint), "disjoint slots should not overlap");
    VP_EXPECT(!disjoint.overlaps(base), "overlaps should be symmetric for disjoint slots");

    //Adjacentes: [9,11) e [11,12)
    //adjacencia NAO conta como sobreposicao.
    const domain::TimeSlot adjacent_after{std::chrono::hours{11}, std::chrono::hours{12}};
    VP_EXPECT(!base.overlaps(adjacent_after), "a slot starting exactly when the other ends should not overlap");
    VP_EXPECT(!adjacent_after.overlaps(base), "overlaps should be symmetric for adjacency");

    const domain::TimeSlot adjacent_before{std::chrono::hours{8}, std::chrono::hours{9}};
    VP_EXPECT(!base.overlaps(adjacent_before), "a slot ending exactly when the other starts should not overlap");
    VP_EXPECT(!adjacent_before.overlaps(base), "overlaps should be symmetric for adjacency, regardless of order");

    //contains: usado por overlaps, mas garantido tambem por si so
    VP_EXPECT(base.contains(std::chrono::hours{9}), "contains should include the start instant");
    VP_EXPECT(!base.contains(std::chrono::hours{11}), "contains should exclude the end instant");
    VP_EXPECT(!base.contains(std::chrono::hours{8}), "contains should exclude instants before start");

    return 0;
}
