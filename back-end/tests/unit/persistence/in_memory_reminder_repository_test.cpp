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

    // O id passado aqui e deliberadamente absurdo: save gera o seu proprio e
    // ignora este (issue #90), do mesmo jeito que InMemoryGoalRepository.
    const domain::Reminder first{
        4242,
        "Study paradigms",
        domain::Category::Study,
        date,
        morning,
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Once};

    const auto first_id = repository.save(first);

    VP_EXPECT(repository.find_all().size() == 1, "repository should hold one reminder after save");
    VP_EXPECT(first_id != 4242, "save must generate the id instead of trusting the caller");
    VP_EXPECT(!repository.find_by_id(4242).has_value(), "the id passed by the caller must not be used");

    const auto stored = repository.find_by_id(first_id);

    VP_EXPECT(stored.has_value(), "saved reminder should be retrievable by the generated id");
    VP_EXPECT(stored->id() == first_id, "the stored entity should carry the generated id");
    VP_EXPECT(stored->description() == "Study paradigms", "description should round-trip");
    VP_EXPECT(stored->category() == domain::Category::Study, "category should round-trip");
    VP_EXPECT(stored->type() == domain::ReminderType::Study, "type should round-trip");
    VP_EXPECT(stored->recurrence() == domain::ReminderRecurrence::Once, "recurrence should round-trip");

    const domain::Reminder second{
        0,
        "Team meeting",
        domain::Category::Work,
        date,
        afternoon,
        domain::ReminderType::Meeting,
        domain::ReminderRecurrence::Weekly};

    const auto second_id = repository.save(second);

    VP_EXPECT(repository.find_all().size() == 2, "repository should hold two reminders");
    VP_EXPECT(second_id != first_id, "each save must produce a distinct id");

    // Regressao da issue #90: antes save fazia upsert, entao salvar de novo
    // com um id ja usado apagava o registro anterior sem aviso. Agora save so
    // insere, e nem uma entidade carregando um id existente sobrescreve nada.
    const domain::Reminder colliding{
        first_id,
        "Should not overwrite",
        domain::Category::Health,
        date,
        afternoon,
        domain::ReminderType::Exercise,
        domain::ReminderRecurrence::Daily};

    const auto third_id = repository.save(colliding);

    VP_EXPECT(repository.find_all().size() == 3, "save must always insert, never overwrite");
    VP_EXPECT(third_id != first_id, "save must not reuse an existing id");
    VP_EXPECT(repository.find_by_id(first_id)->description() == "Study paradigms",
              "saving an entity that carries an existing id must not touch that record");

    // update e a operacao que substitui, e exige o id de quem ja existe.
    const domain::Reminder first_edited{
        first_id,
        "Study paradigms every day",
        domain::Category::Study,
        date,
        afternoon,
        domain::ReminderType::Study,
        domain::ReminderRecurrence::Daily};

    repository.update(first_edited);

    VP_EXPECT(repository.find_all().size() == 3, "update must not append");

    const auto after_update = repository.find_by_id(first_id);

    VP_EXPECT(after_update.has_value(), "updated reminder should still exist");
    VP_EXPECT(after_update->description() == "Study paradigms every day", "update should replace the description");
    VP_EXPECT(after_update->recurrence() == domain::ReminderRecurrence::Daily, "update should replace the recurrence");
    VP_EXPECT(repository.find_by_id(second_id)->recurrence() == domain::ReminderRecurrence::Weekly, "update must not touch other reminders");

    // update de um id inexistente e no-op, como em InMemoryGoalRepository.
    const domain::Reminder unknown{
        999999,
        "Nowhere",
        domain::Category::Leisure,
        date,
        morning,
        domain::ReminderType::Shopping,
        domain::ReminderRecurrence::Once};

    repository.update(unknown);

    VP_EXPECT(repository.find_all().size() == 3, "update of an unknown id must be a no-op");
    VP_EXPECT(!repository.find_by_id(999999).has_value(), "update must not create a reminder");

    repository.remove(first_id);

    VP_EXPECT(repository.find_all().size() == 2, "remove should drop exactly one reminder");
    VP_EXPECT(!repository.find_by_id(first_id).has_value(), "removed reminder should no longer be retrievable");
    VP_EXPECT(repository.find_by_id(second_id).has_value(), "remove must not touch other reminders");

    repository.remove(4242);

    VP_EXPECT(repository.find_all().size() == 2, "remove of an unknown id must be a no-op");

    return 0;
}
