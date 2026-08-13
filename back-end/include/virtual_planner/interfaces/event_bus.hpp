#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::interfaces
{

  struct Event
  {
    std::string name;
    std::string payload;
  };

  class EventBus
  {
  public:
    virtual ~EventBus() = default;

    virtual void publish(const Event &event) = 0;
    [[nodiscard]] virtual bool has_subscribers(std::string_view event_name) const = 0;
  };

}
