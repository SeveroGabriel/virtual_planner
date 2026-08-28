#pragma once

#include <cstdint>
#include <string>

#include "virtual_planner/domain/enums/category.hpp"
#include "virtual_planner/domain/enums/goal_period.hpp"
#include "virtual_planner/domain/enums/goal_status.hpp"
#include "virtual_planner/domain/value_objects/date.hpp"

namespace virtual_planner::domain {

class Goal
{
public:
    Goal(
        std::uint64_t id,
        std::string description,
        Category category,
        GoalStatus status,
        GoalPeriod period,
        Date reference_date
    );

    [[nodiscard]] std::uint64_t id() const;

    [[nodiscard]] const std::string& description() const;

    [[nodiscard]] Category category() const;

    [[nodiscard]] GoalStatus status() const;

    [[nodiscard]] GoalPeriod period() const;

    [[nodiscard]] const Date& reference_date() const;

    void update_description(std::string description);

    void change_category(Category category);

    void change_period(GoalPeriod period);

    void mark_as_in_progress();
    
    void mark_as_completed();

    void mark_as_partially_completed();

    void mark_as_failed();

    void change_reference_date(const Date& reference_date);
    
private:
    std::uint64_t id_;

    std::string description_;

    Category category_;

    GoalStatus status_;

    GoalPeriod period_;

    Date reference_date_;
};

} // namespace virtual_planner::domain