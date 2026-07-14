#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace virtual_planner::interfaces
{

  class Cache
  {
  public:
    virtual ~Cache() = default;

    virtual void put(std::string key, std::string value) = 0;
    [[nodiscard]] virtual std::optional<std::string> get(std::string_view key) const = 0;
    virtual void remove(std::string_view key) = 0;
    virtual void clear() = 0;
  };

}
