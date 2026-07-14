#pragma once

#include <string>
#include <string_view>

namespace virtual_planner::interfaces
{

  class Serializer
  {
  public:
    virtual ~Serializer() = default;

    [[nodiscard]] virtual std::string serialize(std::string_view value) const = 0;
    [[nodiscard]] virtual std::string deserialize(std::string_view value) const = 0;
  };

}
