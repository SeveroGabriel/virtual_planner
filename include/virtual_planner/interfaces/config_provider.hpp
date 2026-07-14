#pragma once

#include "virtual_planner/core/app_config.hpp"

namespace virtual_planner::interfaces
{

  class ConfigProvider
  {
  public:
    virtual ~ConfigProvider() = default;

    [[nodiscard]] virtual core::AppConfig load() const = 0;
  };

}
