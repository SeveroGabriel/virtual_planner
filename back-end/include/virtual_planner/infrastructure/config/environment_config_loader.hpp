#pragma once

#include "virtual_planner/interfaces/config_provider.hpp"

namespace virtual_planner::infrastructure::config
{

  class EnvironmentConfigLoader final : public interfaces::ConfigProvider
  {
  public:
    [[nodiscard]] core::AppConfig load() const override;
  };

}
