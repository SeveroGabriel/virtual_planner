// P-16.2: cobertura de testes para virtual_planner::domain::Task.
#include "virtual_planner/domain/entities/task.hpp"
#include "virtual_planner/shared/errors.hpp"
#include "support/expect.hpp"

#include <chrono>
#include <stdexcept>

using namespace virtual_planner;

namespace
{

domain::Task make_task()
{
    const domain::Date date{15, 8, 2026};
    const domain::TimeSlot time_slot{
        std::chrono::hours{9},
        std::chrono::hours{10}
    };

    return domain::Task{
        42,
        "Study paradigms",
        domain::Category::Study,
        date,
        time_slot,
        domain::Priority::Medium,
        domain::TaskStatus::Pending
    };
}

} // namespace

int main()
{
    // --- Construcao e getters --------------------------------------------
    const domain::Date initial_date{15, 8, 2026};
    const domain::TimeSlot initial_time_slot{
        std::chrono::hours{9},
        std::chrono::hours{10}
    };

    domain::Task task{
        42,
        "Study paradigms",
        domain::Category::Study,
        initial_date,
        initial_time_slot,
        domain::Priority::Medium,
        domain::TaskStatus::Pending
    };

    VP_EXPECT(task.id() == 42, "id should match constructor value");
    VP_EXPECT(task.description() == "Study paradigms", "description should match constructor value");
    VP_EXPECT(task.category() == domain::Category::Study, "category should match constructor value");
    VP_EXPECT(task.date() == initial_date, "date should match constructor value");
    VP_EXPECT(
        task.time_slot().start() == std::chrono::hours{9},
        "time slot start should match constructor value"
    );
    VP_EXPECT(
        task.time_slot().end() == std::chrono::hours{10},
        "time slot end should match constructor value"
    );
    VP_EXPECT(task.priority() == domain::Priority::Medium, "priority should match constructor value");
    VP_EXPECT(task.status() == domain::TaskStatus::Pending, "status should match constructor value");

    // --- Construcao invalida: descricao vazia deve lancar DomainError ------
    bool threw_on_empty_description = false;
    try
    {
        const domain::Task invalid{
            1,
            "",
            domain::Category::Work,
            initial_date,
            initial_time_slot,
            domain::Priority::Low,
            domain::TaskStatus::Pending
        };
    }
    catch (const shared::DomainError&)
    {
        threw_on_empty_description = true;
    }
    VP_EXPECT(threw_on_empty_description, "constructor should reject an empty description");

    // --- Construcao invalida: descricao so com espacos deve lancar ---------
    bool threw_on_blank_description = false;
    try
    {
        const domain::Task invalid{
            1,
            "   ",
            domain::Category::Work,
            initial_date,
            initial_time_slot,
            domain::Priority::Low,
            domain::TaskStatus::Pending
        };
    }
    catch (const shared::DomainError&)
    {
        threw_on_blank_description = true;
    }
    VP_EXPECT(threw_on_blank_description, "constructor should reject a blank (whitespace-only) description");

    // --- Construcao valida: espacos nas bordas com conteudo real sao aceitos
    {
        const domain::Task padded{
            1,
            " Study ",
            domain::Category::Work,
            initial_date,
            initial_time_slot,
            domain::Priority::Low,
            domain::TaskStatus::Pending
        };
        VP_EXPECT(
            padded.description() == " Study ",
            "constructor should accept a non-blank description without trimming it"
        );
    }

    // --- update_description: aceitacao --------------------------------
    {
        domain::Task subject = make_task();
        subject.update_description("Study distributed systems");
        VP_EXPECT(
            subject.description() == "Study distributed systems",
            "update_description should update description when given a non-blank value"
        );
    }

    // --- update_description: rejeicao (vazia) ---------------------------
    {
        domain::Task subject = make_task();
        bool threw = false;
        try
        {
            subject.update_description("");
        }
        catch (const shared::DomainError&)
        {
            threw = true;
        }
        VP_EXPECT(threw, "update_description should reject an empty description");
    }

    // --- update_description: rejeicao (so espacos) -----------------------
    {
        domain::Task subject = make_task();
        bool threw = false;
        try
        {
            subject.update_description("   ");
        }
        catch (const shared::DomainError&)
        {
            threw = true;
        }
        VP_EXPECT(threw, "update_description should reject a blank (whitespace-only) description");
    }

    // --- change_category -----------------------------------------------
    {
        domain::Task subject = make_task();
        subject.change_category(domain::Category::Health);
        VP_EXPECT(subject.category() == domain::Category::Health, "change_category should update category");
    }

    // --- change_date -------------------------------------------------------
    {
        domain::Task subject = make_task();
        const domain::Date new_date{1, 1, 2027};
        subject.change_date(new_date);
        VP_EXPECT(subject.date() == new_date, "change_date should update date");
    }

    // --- change_time_slot ----------------------------------------------
    {
        domain::Task subject = make_task();
        const domain::TimeSlot new_time_slot{
            std::chrono::hours{14},
            std::chrono::hours{15}
        };
        subject.change_time_slot(new_time_slot);
        VP_EXPECT(
            subject.time_slot().start() == std::chrono::hours{14},
            "change_time_slot should update time slot start"
        );
        VP_EXPECT(
            subject.time_slot().end() == std::chrono::hours{15},
            "change_time_slot should update time slot end"
        );
    }

    // --- change_priority -----------------------------------------------
    {
        domain::Task subject = make_task();
        subject.change_priority(domain::Priority::High);
        VP_EXPECT(subject.priority() == domain::Priority::High, "change_priority should update priority");
    }

    // --- mark_as_pending -----------------------------------------------
    {
        domain::Task subject = make_task();
        subject.mark_as_executed();
        subject.mark_as_pending();
        VP_EXPECT(subject.status() == domain::TaskStatus::Pending, "mark_as_pending should set status to Pending");
    }

    // --- mark_as_executed ----------------------------------------------
    {
        domain::Task subject = make_task();
        subject.mark_as_executed();
        VP_EXPECT(subject.status() == domain::TaskStatus::Executed, "mark_as_executed should set status to Executed");
    }

    // --- mark_as_partially_executed --------------------------------------
    {
        domain::Task subject = make_task();
        subject.mark_as_partially_executed();
        VP_EXPECT(
            subject.status() == domain::TaskStatus::PartiallyExecuted,
            "mark_as_partially_executed should set status to PartiallyExecuted"
        );
    }

    // --- mark_as_cancelled ---------------------------------------------
    {
        domain::Task subject = make_task();
        subject.mark_as_cancelled();
        VP_EXPECT(subject.status() == domain::TaskStatus::Cancelled, "mark_as_cancelled should set status to Cancelled");
    }

    // --- mark_as_postponed ---------------------------------------------
    {
        domain::Task subject = make_task();
        subject.mark_as_postponed();
        VP_EXPECT(subject.status() == domain::TaskStatus::Postponed, "mark_as_postponed should set status to Postponed");
    }

    // --- transicao encadeada: nao ha maquina de estados restringindo -------
    {
        domain::Task subject = make_task();
        subject.mark_as_executed();
        subject.mark_as_cancelled();
        VP_EXPECT(
            subject.status() == domain::TaskStatus::Cancelled,
            "status transitions should be free-form (no state machine constraint in this issue's scope)"
        );
    }

    return 0;
}
