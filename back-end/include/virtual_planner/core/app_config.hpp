#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace virtual_planner::core
{

  enum class ExecutionProfile
  {
    Development,
    Test,
    Production,
  };

  ExecutionProfile parse_execution_profile(std::string_view value);
  std::string_view to_string(ExecutionProfile profile);

  class AppConfig
  {
  public:
    AppConfig();
    AppConfig(std::string app_name, ExecutionProfile profile);

    [[nodiscard]] const std::string &app_name() const noexcept;
    [[nodiscard]] ExecutionProfile profile() const noexcept;

    void set(std::string key, std::string value);
    [[nodiscard]] std::optional<std::string> get(std::string_view key) const;
    [[nodiscard]] std::string get_or(std::string_view key, std::string fallback) const;

  private:
    std::string app_name_;
    ExecutionProfile profile_;
    std::unordered_map<std::string, std::string> values_;
  };

}