#include "virtual_planner/domain/entities/goal.hpp"

#include <utility>
#include <stdexcept>

namespace virtual_planner::domain {

Goal::Goal(
    std::uint64_t id,
    std::string description,
    Category category,
    GoalStatus status,
    GoalPeriod period
)
    : id_(id),
      description_(std::move(description)),
      category_(category),
      status_(status),
      period_(period)
{
     if (description_.empty())
    {
        throw std::invalid_argument(
            "Goal description cannot be empty."
        );
    }
}

std::uint64_t Goal::id() const
{
    return id_;
}

const std::string& Goal::description() const
{
    return description_;
}

Category Goal::category() const
{
    return category_;
}

GoalStatus Goal::status() const
{
    return status_;
}

GoalPeriod Goal::period() const
{
    return period_;
}

void Goal::update_description(std::string description)
{
    if (description.empty())
    {
        throw std::invalid_argument(
            "Goal description cannot be empty."
        );
    }

    description_ = std::move(description);
}

void Goal::change_category(Category category)
{
    category_ = category;
}

void Goal::change_period(GoalPeriod period)
{
    period_ = period;
}

void Goal::mark_as_in_progress()
{
    status_ = GoalStatus::InProgress;
}

void Goal::mark_as_completed()
{
    status_ = GoalStatus::Completed;
}

void Goal::mark_as_partially_completed()
{
    status_ = GoalStatus::PartiallyCompleted;
}

void Goal::mark_as_failed()
{
    status_ = GoalStatus::Failed;
}

} // namespace virtual_planner::domain