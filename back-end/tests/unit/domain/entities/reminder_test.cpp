#include "virtual_planner/domain/entities/reminder.hpp"
#include "virtual_planner/shared/errors.hpp"
#include "support/expect.hpp"

#include <chrono>

using namespace virtual_planner;

int main()
{
    const domain::Date initial_date{15, 8, 2026};
    const domain::TimeSlot initial_time{
        std::chrono::hours{9},
        std::chrono::hours{10}
    };

    domain::Reminder reminder{
        42,
        "Study paradigms",
        domain::Category::Study,
        initial_date,
        initial_time,
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Once
    };

    VP_EXPECT(reminder.id() == 42, "id should match constructor value");
    VP_EXPECT(reminder.description() == "Study paradigms", "description should match constructor value");
    VP_EXPECT(reminder.category() == domain::Category::Study, "category should match constructor value");
    VP_EXPECT(reminder.date() == initial_date, "date should match constructor value");

    VP_EXPECT(
        reminder.time_slot().start() == std::chrono::hours{9},
        "time slot start should match constructor value"
    );

    VP_EXPECT(
        reminder.time_slot().end() == std::chrono::hours{10},
        "time slot end should match constructor value"
    );

    VP_EXPECT(reminder.type() == domain::ReminderType::Study, "type should match constructor value");

    VP_EXPECT(
        reminder.recurrence() == domain::ReminderRecurrence::Once,
        "recurrence should match constructor value"
    );

    reminder.update_description("Weekly meeting");

    VP_EXPECT(
        reminder.description() == "Weekly meeting",
        "update_description should update description"
    );

    reminder.change_category(domain::Category::Work);

    VP_EXPECT(
        reminder.category() == domain::Category::Work,
        "change_category should update category"
    );

    const domain::Date new_date{22, 8, 2026};
    reminder.change_date(new_date);

    VP_EXPECT(
        reminder.date() == new_date,
        "change_date should update date"
    );

    const domain::TimeSlot new_time{
        std::chrono::hours{14},
        std::chrono::hours{15}
    };

    reminder.change_time_slot(new_time);

    VP_EXPECT(
        reminder.time_slot().start() == std::chrono::hours{14},
        "change_time_slot should update start time"
    );

    VP_EXPECT(
        reminder.time_slot().end() == std::chrono::hours{15},
        "change_time_slot should update end time"
    );

    reminder.change_type(domain::ReminderType::Meeting);

    VP_EXPECT(
        reminder.type() == domain::ReminderType::Meeting,
        "change_type should update type"
    );

    reminder.change_recurrence(domain::ReminderRecurrence::Weekly);

    VP_EXPECT(
        reminder.recurrence() == domain::ReminderRecurrence::Weekly,
        "change_recurrence should update recurrence"
    );

    bool empty_constructor_rejected = false;

    try
    {
        domain::Reminder invalid{
            1,
            "",
            domain::Category::Study,
            initial_date,
            initial_time,
            domain::ReminderType::Study,
            domain::ReminderRecurrence::Once
        };
    }
    catch (const shared::DomainError&)
    {
        empty_constructor_rejected = true;
    }

    VP_EXPECT(
        empty_constructor_rejected,
        "constructor should reject an empty description"
    );

    bool blank_constructor_rejected = false;

    try
    {
        domain::Reminder invalid{
            2,
            "   ",
            domain::Category::Study,
            initial_date,
            initial_time,
            domain::ReminderType::Study,
            domain::ReminderRecurrence::Once
        };
    }
    catch (const shared::DomainError&)
    {
        blank_constructor_rejected = true;
    }

    VP_EXPECT(
        blank_constructor_rejected,
        "constructor should reject a blank description"
    );

    bool empty_update_rejected = false;

    try
    {
        reminder.update_description("");
    }
    catch (const shared::DomainError&)
    {
        empty_update_rejected = true;
    }

    VP_EXPECT(
        empty_update_rejected,
        "update_description should reject an empty description"
    );

    bool blank_update_rejected = false;

    try
    {
        reminder.update_description("   ");
    }
    catch (const shared::DomainError&)
    {
        blank_update_rejected = true;
    }

    VP_EXPECT(
        blank_update_rejected,
        "update_description should reject a blank description"
    );

    VP_EXPECT(
        reminder.description() == "Weekly meeting",
        "rejected updates should preserve the previous description"
    );

    return 0;
}
