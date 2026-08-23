// P-15.1: cobertura de testes para virtual_planner::domain::Date.
#include "virtual_planner/domain/value_objects/date.hpp"
#include "support/expect.hpp"

#include <stdexcept>

using namespace virtual_planner;

namespace
{

bool throws_invalid_argument(std::uint32_t day, std::uint32_t month, std::uint32_t year)
{
    try
    {
        domain::Date invalid{day, month, year};
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
    // --- Caso feliz: data valida generica -----------------------------------
    const domain::Date date{15, 8, 2026};

    VP_EXPECT(date.day() == 15, "day should match constructor value");
    VP_EXPECT(date.month() == 8, "month should match constructor value");
    VP_EXPECT(date.year() == 2026, "year should match constructor value");
    VP_EXPECT(date.to_string() == "15/08/2026", "to_string should format as dd/mm/yyyy");

    // --- Limites de dia: primeiro e ultimo dia de meses de 31, 30 e 28/29 ---
    const domain::Date first_day_of_month{1, 1, 2026};
    VP_EXPECT(first_day_of_month.day() == 1, "day 1 should be accepted");

    const domain::Date last_day_31{31, 1, 2026};
    VP_EXPECT(last_day_31.day() == 31, "day 31 should be accepted for a 31-day month");

    const domain::Date last_day_30{30, 4, 2026};
    VP_EXPECT(last_day_30.day() == 30, "day 30 should be accepted for a 30-day month");

    const domain::Date last_day_february_non_leap{28, 2, 2023};
    VP_EXPECT(
        last_day_february_non_leap.day() == 28,
        "day 28 should be accepted for February in a non-leap year"
    );

    const domain::Date last_day_february_leap{29, 2, 2024};
    VP_EXPECT(
        last_day_february_leap.day() == 29,
        "day 29 should be accepted for February in a leap year"
    );

    // --- Limite de ano: 1900 e o menor ano aceito ----------------------------
    const domain::Date minimum_year{1, 1, 1900};
    VP_EXPECT(minimum_year.year() == 1900, "year 1900 should be the minimum accepted year");

    // --- Ano bissexto: regra gregoriana completa -----------------------------
    // Divisivel por 4 e nao por 100: bissexto (ex.: 2024).
    VP_EXPECT(
        !throws_invalid_argument(29, 2, 2024),
        "a year divisible by 4 and not by 100 should be a leap year"
    );

    // Divisivel por 100 e nao por 400: nao bissexto (ex.: 1900).
    VP_EXPECT(
        throws_invalid_argument(29, 2, 1900),
        "a year divisible by 100 but not by 400 should not be a leap year"
    );

    // Divisivel por 400: bissexto (ex.: 2000).
    VP_EXPECT(
        !throws_invalid_argument(29, 2, 2000),
        "a year divisible by 400 should be a leap year"
    );

    // Nao divisivel por 4: nao bissexto (ex.: 2023).
    VP_EXPECT(
        throws_invalid_argument(29, 2, 2023),
        "a year not divisible by 4 should not be a leap year"
    );

    // --- Cada throw de date.cpp precisa de ao menos um caso ------------------

    // month == 0
    VP_EXPECT(throws_invalid_argument(1, 0, 2026), "month 0 should be rejected");

    // month > 12
    VP_EXPECT(throws_invalid_argument(1, 13, 2026), "month 13 should be rejected");

    // year < 1900
    VP_EXPECT(throws_invalid_argument(1, 1, 1899), "year before 1900 should be rejected");

    // day == 0
    VP_EXPECT(throws_invalid_argument(0, 1, 2026), "day 0 should be rejected");

    // day > days_in_month, para meses de 31, 30 e 28/29 dias
    VP_EXPECT(throws_invalid_argument(32, 1, 2026), "day 32 should be rejected for a 31-day month");
    VP_EXPECT(throws_invalid_argument(31, 4, 2026), "day 31 should be rejected for a 30-day month");
    VP_EXPECT(throws_invalid_argument(29, 2, 2023), "day 29 should be rejected for February in a non-leap year");
    VP_EXPECT(throws_invalid_argument(30, 2, 2024), "day 30 should be rejected for February in a leap year");

    // --- Operadores de comparacao --------------------------------------
    const domain::Date same_as_date{15, 8, 2026};
    const domain::Date earlier_day{14, 8, 2026};
    const domain::Date later_day{16, 8, 2026};
    const domain::Date earlier_month{15, 7, 2026};
    const domain::Date later_month{15, 9, 2026};
    const domain::Date earlier_year{15, 8, 2025};
    const domain::Date later_year{15, 8, 2027};

    // operator==
    VP_EXPECT(date == same_as_date, "operator== should be true for equal dates");
    VP_EXPECT(!(date == later_day), "operator== should be false for a different day");

    // operator!=
    VP_EXPECT(date != later_day, "operator!= should be true for a different day");
    VP_EXPECT(!(date != same_as_date), "operator!= should be false for equal dates");

    // operator< (ano, depois mes, depois dia)
    VP_EXPECT(earlier_year < date, "operator< should compare year first");
    VP_EXPECT(earlier_month < date, "operator< should compare month when year is equal");
    VP_EXPECT(earlier_day < date, "operator< should compare day when year and month are equal");
    VP_EXPECT(!(date < date), "operator< should be false for equal dates");
    VP_EXPECT(!(later_day < date), "operator< should be false when the date is later");

    // operator>
    VP_EXPECT(later_year > date, "operator> should compare year first");
    VP_EXPECT(later_month > date, "operator> should compare month when year is equal");
    VP_EXPECT(later_day > date, "operator> should compare day when year and month are equal");
    VP_EXPECT(!(date > date), "operator> should be false for equal dates");
    VP_EXPECT(!(earlier_day > date), "operator> should be false when the date is earlier");

    // operator<=
    VP_EXPECT(earlier_day <= date, "operator<= should be true when the date is earlier");
    VP_EXPECT(date <= same_as_date, "operator<= should be true for equal dates");
    VP_EXPECT(!(later_day <= date), "operator<= should be false when the date is later");

    // operator>=
    VP_EXPECT(later_day >= date, "operator>= should be true when the date is later");
    VP_EXPECT(date >= same_as_date, "operator>= should be true for equal dates");
    VP_EXPECT(!(earlier_day >= date), "operator>= should be false when the date is earlier");

    return 0;
}
