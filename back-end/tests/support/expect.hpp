#pragma once

// Minimal runtime assertion helper for the project's CTest executables.
//
// Rationale: <cassert>'s assert() is compiled out when NDEBUG is defined
// (CMAKE_BUILD_TYPE=Release), which would make every test silently pass
// without verifying anything. The helpers below always evaluate the
// condition and always report failures, regardless of NDEBUG.
//
// Usage:
//   #include "support/expect.hpp"
//   VP_EXPECT(goal.has_value(), "goal must exist after save");
//
// On failure the message, expression and source location are printed to
// stderr and the process exits with a non-zero status, so CTest reports
// the test as failed.

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace virtual_planner::tests {

inline void expect(
    bool condition,
    std::string_view message,
    std::string_view expression,
    std::string_view file,
    int line)
{
    if (condition)
    {
        return;
    }

    std::cerr << "EXPECTATION FAILED\n"
              << "  message:    " << message << '\n'
              << "  expression: " << expression << '\n'
              << "  location:   " << file << ':' << line << '\n';
    std::cerr.flush();

    std::exit(EXIT_FAILURE);
}

} // namespace virtual_planner::tests

#define VP_EXPECT(condition, message)                                          \
    ::virtual_planner::tests::expect(                                          \
        static_cast<bool>(condition), (message), #condition, __FILE__, __LINE__)
