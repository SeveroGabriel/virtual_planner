#pragma once

#include <stdexcept>
#include <string>

namespace virtual_planner::shared
{

  class ApplicationError : public std::runtime_error
  {
  public:
    explicit ApplicationError(const std::string &message);
  };

  class DomainError final : public ApplicationError
  {
  public:
    explicit DomainError(const std::string &message);
  };

  class ConfigError final : public ApplicationError
  {
  public:
    explicit ConfigError(const std::string &message);
  };

  class PersistenceError final : public ApplicationError
  {
  public:
    explicit PersistenceError(const std::string &message);
  };

  class InitializationError final : public ApplicationError
  {
  public:
    explicit InitializationError(const std::string &message);
  };

}
