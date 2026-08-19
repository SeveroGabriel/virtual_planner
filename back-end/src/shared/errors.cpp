#include "virtual_planner/shared/errors.hpp"

namespace virtual_planner::shared
{

    ApplicationError::ApplicationError(const std::string &message)
        : std::runtime_error(message) {}

    DomainError::DomainError(const std::string &message)
        : ApplicationError(message) {}

    ConfigError::ConfigError(const std::string &message) : ApplicationError(message) {}

    PersistenceError::PersistenceError(const std::string &message)
        : ApplicationError(message) {}

    InitializationError::InitializationError(const std::string &message)
        : ApplicationError(message) {}

}
