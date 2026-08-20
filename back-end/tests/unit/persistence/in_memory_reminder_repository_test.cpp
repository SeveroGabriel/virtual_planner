#include "virtual_planner/persistence/memory/in_memory_reminder_repository.hpp"

#include "support/expect.hpp"

#include <chrono>

using namespace virtual_planner;

int main()
{
    persistence::InMemoryReminderRepository repository;

    const domain::Date date{15, 8, 2026};
    const domain::TimeSlot morning{std::chrono::hours{9}, std::chrono::hours{10}};
    const domain::TimeSlot afternoon{std::chrono::hours{14}, std::chrono::hours{15}};

    VP_EXPECT(repository.find_all().empty(), "repository should start empty");
    VP_EXPECT(!repository.find_by_id(1).has_value(), "find_by_id should return nullopt when empty");

    const domain::Reminder first{
        1,
        "Study paradigms",
        domain::Category::Study,
        date,
        morning,
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Once};

    repository.save(first);

    VP_EXPECT(repository.find_all().size() == 1, "repository should hold one reminder after save");

    const auto stored = repository.find_by_id(1);

    VP_EXPECT(stored.has_value(), "saved reminder should be retrievable by its own id");
    VP_EXPECT(stored->id() == 1, "save must preserve the entity id");
    VP_EXPECT(stored->description() == "Study paradigms", "description should round-trip");
    VP_EXPECT(stored->category() == domain::Category::Study, "category should round-trip");
    VP_EXPECT(stored->type() == domain::ReminderType::Study, "type should round-trip");
    VP_EXPECT(stored->recurrence() == domain::ReminderRecurrence::Once, "recurrence should round-trip");

    const domain::Reminder second{
        2,
        "Team meeting",
        domain::Category::Work,
        date,
        afternoon,
        domain::ReminderType::Meeting,
        domain::ReminderRecurrence::Weekly};

    repository.save(second);

    VP_EXPECT(repository.find_all().size() == 2, "repository should hold two reminders");

    // Recorrencia e o caso que a P-21 exercita: alterar sem update no contrato.
    const domain::Reminder first_edited{
        1,
        "Study paradigms every day",
        domain::Category::Study,
        date,
        afternoon,
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Daily};

    repository.save(first_edited);

    VP_EXPECT(repository.find_all().size() == 2, "save with an existing id must upsert, not append");

    const auto after_upsert = repository.find_by_id(1);

    VP_EXPECT(after_upsert.has_value(), "upserted reminder should still exist");
    VP_EXPECT(after_upsert->description() == "Study paradigms every day", "upsert should replace the description");
    VP_EXPECT(after_upsert->recurrence() == domain::ReminderRecurrence::Daily, "upsert should replace the recurrence");
    VP_EXPECT(repository.find_by_id(2)->recurrence() == domain::ReminderRecurrence::Weekly, "upsert must not touch other reminders");

    repository.remove(1);

    VP_EXPECT(repository.find_all().size() == 1, "remove should drop exactly one reminder");
    VP_EXPECT(!repository.find_by_id(1).has_value(), "removed reminder should no longer be retrievable");
    VP_EXPECT(repository.find_by_id(2).has_value(), "remove must not touch other reminders");

    repository.remove(4242);

    VP_EXPECT(repository.find_all().size() == 1, "remove of an unknown id must be a no-op");

    return 0;
}
