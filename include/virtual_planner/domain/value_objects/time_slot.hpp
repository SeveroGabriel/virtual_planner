#pragma once

#include <chrono>

namespace virtual_planner::domain {

class TimeSlot
{
public:
    using Minutes = std::chrono::minutes;

    TimeSlot(Minutes start, Minutes end);

    [[nodiscard]] Minutes start() const;

    [[nodiscard]] Minutes end() const;

    [[nodiscard]] bool overlaps(const TimeSlot& other) const;

    [[nodiscard]] bool contains(Minutes time) const;

    [[nodiscard]] Minutes duration() const;

private:
    Minutes start_;
    Minutes end_;
};

} // namespace virtual_planner::domain