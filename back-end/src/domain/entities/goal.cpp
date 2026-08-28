#include "virtual_planner/domain/entities/goal.hpp"

#include <stdexcept>
#include <utility>

namespace virtual_planner::domain
{

namespace
{

bool is_blank(const std::string& value)
{
    return value.find_first_not_of(" \t\n\r\f\v") == std::string::npos;
}

} // namespace

Goal::Goal(
    std::uint64_t id,
    std::string description,
    Category category,
    GoalStatus status,
    GoalPeriod period,
    Date reference_date
)
    : id_(id),
      description_(std::move(description)),
      category_(category),
      status_(status),
      period_(period),
      reference_date_(reference_date)
{
    if (is_blank(description_))
    {
        throw std::invalid_argument(
            "Goal description cannot be empty or blank.");
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

const Date& Goal::reference_date() const
{
    return reference_date_;
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
    if (is_blank(description))
    {
        throw std::invalid_argument(
            "Goal description cannot be empty or blank.");
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

void Goal::change_reference_date(const Date& reference_date)
{
    reference_date_ = reference_date;
}

} // namespace virtual_planner::domain
