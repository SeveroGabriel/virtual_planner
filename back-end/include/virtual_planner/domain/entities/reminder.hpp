#pragma once

#include <cstdint>
#include <string>

#include "virtual_planner/domain/enums/category.hpp"
#include "virtual_planner/domain/enums/reminder_recurrence.hpp"
#include "virtual_planner/domain/enums/reminder_type.hpp"
#include "virtual_planner/domain/value_objects/date.hpp"
#include "virtual_planner/domain/value_objects/time_slot.hpp"

namespace virtual_planner::domain {

class Reminder
{
public:
    Reminder(
        std::uint64_t id,
        std::string description,
        Category category,
        Date date,
        TimeSlot time_slot,
        ReminderType type,
        ReminderRecurrence recurrence
    );

    [[nodiscard]] std::uint64_t id() const;

    [[nodiscard]] const std::string& description() const;

    [[nodiscard]] Category category() const;

    [[nodiscard]] Date date() const;

    [[nodiscard]] TimeSlot time_slot() const;

    [[nodiscard]] ReminderType type() const;

    [[nodiscard]] ReminderRecurrence recurrence() const;

    void update_description(std::string description);

    void change_category(Category category);

    void change_date(Date date);

    void change_time_slot(TimeSlot time_slot);

    void change_type(ReminderType type);

    void change_recurrence(ReminderRecurrence recurrence);

private:
    std::uint64_t id_;

    std::string description_;

    Category category_;

    Date date_;

    TimeSlot time_slot_;

    ReminderType type_;

    ReminderRecurrence recurrence_;
};

} // namespace virtual_planner::domain