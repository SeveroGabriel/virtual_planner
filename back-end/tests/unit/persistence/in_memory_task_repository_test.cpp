#include "virtual_planner/persistence/memory/in_memory_task_repository.hpp"

#include "support/expect.hpp"

#include <chrono>

using namespace virtual_planner;

int main()
{
    persistence::InMemoryTaskRepository repository;

    const domain::Date date{15, 8, 2026};
    const domain::TimeSlot morning{std::chrono::hours{9}, std::chrono::hours{10}};
    const domain::TimeSlot afternoon{std::chrono::hours{14}, std::chrono::hours{15}};

    VP_EXPECT(repository.find_all().empty(), "repository should start empty");
    VP_EXPECT(!repository.find_by_id(1).has_value(), "find_by_id should return nullopt when empty");

    const domain::Task first{
        1,
        "Write the report",
        domain::Category::Work,
        date,
        morning,
        domain::Priority::High,
        domain::TaskStatus::Pending};

    repository.save(first);

    VP_EXPECT(repository.find_all().size() == 1, "repository should hold one task after save");

    const auto stored = repository.find_by_id(1);

    VP_EXPECT(stored.has_value(), "saved task should be retrievable by its own id");
    VP_EXPECT(stored->id() == 1, "save must preserve the entity id");
    VP_EXPECT(stored->description() == "Write the report", "description should round-trip");
    VP_EXPECT(stored->category() == domain::Category::Work, "category should round-trip");
    VP_EXPECT(stored->priority() == domain::Priority::High, "priority should round-trip");
    VP_EXPECT(stored->status() == domain::TaskStatus::Pending, "status should round-trip");

    const domain::Task second{
        2,
        "Review the pull request",
        domain::Category::College,
        date,
        afternoon,
        domain::Priority::Medium,
        domain::TaskStatus::Pending};

    repository.save(second);

    VP_EXPECT(repository.find_all().size() == 2, "repository should hold two tasks");

    // Sem update no contrato, save precisa sobrescrever quem tem o mesmo id.
    const domain::Task first_edited{
        1,
        "Write the final report",
        domain::Category::Work,
        date,
        afternoon,
        domain::Priority::Low,
        domain::TaskStatus::Executed};

    repository.save(first_edited);

    VP_EXPECT(repository.find_all().size() == 2, "save with an existing id must upsert, not append");

    const auto after_upsert = repository.find_by_id(1);

    VP_EXPECT(after_upsert.has_value(), "upserted task should still exist");
    VP_EXPECT(after_upsert->description() == "Write the final report", "upsert should replace the description");
    VP_EXPECT(after_upsert->priority() == domain::Priority::Low, "upsert should replace the priority");
    VP_EXPECT(after_upsert->status() == domain::TaskStatus::Executed, "upsert should replace the status");
    VP_EXPECT(repository.find_by_id(2)->description() == "Review the pull request", "upsert must not touch other tasks");

    repository.remove(1);

    VP_EXPECT(repository.find_all().size() == 1, "remove should drop exactly one task");
    VP_EXPECT(!repository.find_by_id(1).has_value(), "removed task should no longer be retrievable");
    VP_EXPECT(repository.find_by_id(2).has_value(), "remove must not touch other tasks");

    repository.remove(4242);

    VP_EXPECT(repository.find_all().size() == 1, "remove of an unknown id must be a no-op");

    return 0;
}
