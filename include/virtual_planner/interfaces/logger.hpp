#pragma once

#include <string_view>

namespace virtual_planner::interfaces
{

  class Logger
  {
  public:
    virtual ~Logger() = default;

    virtual void debug(std::string_view message) = 0;
    virtual void info(std::string_view message) = 0;
    virtual void warn(std::string_view message) = 0;
    virtual void error(std::string_view message) = 0;
  };

}