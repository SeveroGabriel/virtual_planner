#include "virtual_planner/infrastructure/logging/console_logger.hpp"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

namespace virtual_planner::infrastructure::logging {

namespace {

std::string utc_timestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t seconds = std::chrono::system_clock::to_time_t(now);

    std::tm parts{};
#if defined(_WIN32)
    gmtime_s(&parts, &seconds);
#else
    gmtime_r(&seconds, &parts);
#endif

    char buffer[sizeof("2026-08-28T16:00:00Z")];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &parts);

    return buffer;
}

} // namespace

ConsoleLogger::ConsoleLogger(interfaces::LogLevel minimum)
    : minimum_(minimum)
{
}

interfaces::LogLevel ConsoleLogger::level_from_environment()
{
    const char* value = std::getenv("VP_LOG_LEVEL");

    if (value == nullptr || *value == '\0')
    {
        return interfaces::LogLevel::Info;
    }

    return interfaces::log_level_from_string(value)
        .value_or(interfaces::LogLevel::Info);
}

interfaces::LogLevel ConsoleLogger::minimum() const noexcept
{
    return minimum_;
}

void ConsoleLogger::log(interfaces::LogLevel level,
                        std::string_view message,
                        std::string_view fields)
{
    if (static_cast<int>(level) < static_cast<int>(minimum_))
    {
        return;
    }

    std::ostream& stream =
        level == interfaces::LogLevel::Error ? std::cerr : std::cout;

    const std::lock_guard<std::mutex> guard(mutex_);

    stream << utc_timestamp() << ' ' << interfaces::to_string(level) << ' '
           << message;

    if (!fields.empty())
    {
        stream << ' ' << fields;
    }

    stream << '\n';
    stream.flush();
}

} // namespace virtual_planner::infrastructure::logging
