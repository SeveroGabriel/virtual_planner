#include "virtual_planner/domain/value_objects/time_slot.hpp"

#include <stdexcept>

namespace virtual_planner::domain {

TimeSlot::TimeSlot(Minutes start, Minutes end)
    : start_(start), end_(end)
{
    if (end <= start)
    {
        throw std::invalid_argument(
            "End time must be after start time.");
    }
}

Minutes TimeSlot::start() const
{
    return start_;
}

Minutes TimeSlot::end() const
{
    return end_;
}

Minutes TimeSlot::duration() const
{
    return end_ - start_;
}

bool TimeSlot::contains(Minutes time) const
{
   return time >= start_ && time < end_;
}

bool TimeSlot::overlaps(const TimeSlot& other) const
{
    return start_ < other.end_ &&
           end_ > other.start_;
}

} // namespace virtual_planner::domain