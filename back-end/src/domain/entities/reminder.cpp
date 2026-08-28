#include "virtual_planner/domain/entities/reminder.hpp"
#include "virtual_planner/domain/text.hpp"
#include "virtual_planner/shared/errors.hpp"

#include <utility>

namespace virtual_planner::domain {

namespace {

void validate_description(const std::string& description)
{
    if (is_blank(description))
    {
        throw shared::DomainError(
            "Reminder description cannot be empty or blank."
        );
    }
}

}

Reminder::Reminder(
    std::uint64_t id,
    std::string description,
    Category category,
    Date date,
    TimeSlot time_slot,
    ReminderType type,
    ReminderRecurrence recurrence
)
    : id_(id),
      description_(std::move(description)),
      category_(category),
      date_(date),
      time_slot_(time_slot),
      type_(type),
      recurrence_(recurrence)
{
    validate_description(description_);
}

std::uint64_t Reminder::id() const
{
    return id_;
}

const std::string& Reminder::description() const
{
    return description_;
}

Category Reminder::category() const
{
    return category_;
}

Date Reminder::date() const
{
    return date_;
}

TimeSlot Reminder::time_slot() const
{
    return time_slot_;
}

ReminderType Reminder::type() const
{
    return type_;
}

ReminderRecurrence Reminder::recurrence() const
{
    return recurrence_;
}

void Reminder::update_description(std::string description)
{
    validate_description(description);
    description_ = std::move(description);
}

void Reminder::change_category(Category category)
{
    category_ = category;
}

void Reminder::change_date(Date date)
{
    date_ = date;
}

void Reminder::change_time_slot(TimeSlot time_slot)
{
    time_slot_ = time_slot;
}

void Reminder::change_type(ReminderType type)
{
    type_ = type;
}

void Reminder::change_recurrence(ReminderRecurrence recurrence)
{
    recurrence_ = recurrence;
}

}
