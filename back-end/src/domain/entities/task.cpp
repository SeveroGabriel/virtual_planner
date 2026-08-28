#include "virtual_planner/domain/entities/task.hpp"
#include "virtual_planner/domain/text.hpp"
#include "virtual_planner/shared/errors.hpp"

#include <stdexcept>
#include <utility>

namespace virtual_planner::domain {

namespace {

void validate_description(const std::string& description)
{
    if (is_blank(description))
    {
        throw shared::DomainError(
            "Task description cannot be empty or blank."
        );
    }
}

}

Task::Task(
    std::uint64_t id,
    std::string description,
    Category category,
    Date date,
    TimeSlot time_slot,
    Priority priority,
    TaskStatus status
)
    : id_(id),
      description_(std::move(description)),
      category_(category),
      date_(date),
      time_slot_(time_slot),
      priority_(priority),
      status_(status)
{
    validate_description(description_);
}

std::uint64_t Task::id() const
{
    return id_;
}

const std::string& Task::description() const
{
    return description_;
}

Category Task::category() const
{
    return category_;
}

Date Task::date() const
{
    return date_;
}

TimeSlot Task::time_slot() const
{
    return time_slot_;
}

Priority Task::priority() const
{
    return priority_;
}

TaskStatus Task::status() const
{
    return status_;
}

void Task::update_description(std::string description)
{
    validate_description(description);
    description_ = std::move(description);
}

void Task::change_category(Category category)
{
    category_ = category;
}

void Task::change_date(Date date)
{
    date_ = date;
}

void Task::change_time_slot(TimeSlot time_slot)
{
    time_slot_ = time_slot;
}

void Task::change_priority(Priority priority)
{
    priority_ = priority;
}

void Task::mark_as_pending()
{
    status_ = TaskStatus::Pending;
}

void Task::mark_as_executed()
{
    status_ = TaskStatus::Executed;
}

void Task::mark_as_partially_executed()
{
    status_ = TaskStatus::PartiallyExecuted;
}

void Task::mark_as_cancelled()
{
    status_ = TaskStatus::Cancelled;
}

void Task::mark_as_postponed()
{
    status_ = TaskStatus::Postponed;
}

} // namespace virtual_planner::domain