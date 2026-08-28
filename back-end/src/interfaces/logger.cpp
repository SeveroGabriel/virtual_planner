#include "virtual_planner/interfaces/logger.hpp"

namespace virtual_planner::interfaces {

std::string_view to_string(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Debug:
            return "DEBUG";

        case LogLevel::Info:
            return "INFO";

        case LogLevel::Warning:
            return "WARN";

        case LogLevel::Error:
            return "ERROR";
    }

    return "INFO";
}

std::optional<LogLevel> log_level_from_string(std::string_view value)
{
    if (value == "debug" || value == "DEBUG") return LogLevel::Debug;
    if (value == "info" || value == "INFO") return LogLevel::Info;
    if (value == "warning" || value == "WARN" || value == "WARNING")
        return LogLevel::Warning;
    if (value == "error" || value == "ERROR") return LogLevel::Error;

    return std::nullopt;
}

void Logger::debug(std::string_view message, std::string_view fields)
{
    log(LogLevel::Debug, message, fields);
}

void Logger::info(std::string_view message, std::string_view fields)
{
    log(LogLevel::Info, message, fields);
}

void Logger::warning(std::string_view message, std::string_view fields)
{
    log(LogLevel::Warning, message, fields);
}

void Logger::error(std::string_view message, std::string_view fields)
{
    log(LogLevel::Error, message, fields);
}

} // namespace virtual_planner::interfaces
