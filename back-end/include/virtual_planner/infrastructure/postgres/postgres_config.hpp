#pragma once

#include "virtual_planner/core/app_config.hpp"

#include <cstdint>
#include <string>

namespace virtual_planner::infrastructure::postgres
{

  class PostgresConfig final
  {
  public:
    PostgresConfig(std::string host,
                   std::uint16_t port,
                   std::string database,
                   std::string user,
                   std::string password,
                   std::string sslmode,
                   std::uint16_t connect_timeout,
                   std::string application_name);

    [[nodiscard]] static PostgresConfig from_app_config(const core::AppConfig &config);
    [[nodiscard]] static PostgresConfig from_environment();

    [[nodiscard]] const std::string &host() const noexcept;
    [[nodiscard]] std::uint16_t port() const noexcept;
    [[nodiscard]] const std::string &database() const noexcept;
    [[nodiscard]] const std::string &user() const noexcept;
    [[nodiscard]] const std::string &password() const noexcept;
    [[nodiscard]] const std::string &sslmode() const noexcept;
    [[nodiscard]] std::uint16_t connect_timeout() const noexcept;
    [[nodiscard]] const std::string &application_name() const noexcept;

    void validate() const;
    [[nodiscard]] std::string connection_string() const;
    [[nodiscard]] std::string masked_connection_string() const;

  private:
    std::string host_;
    std::uint16_t port_;
    std::string database_;
    std::string user_;
    std::string password_;
    std::string sslmode_;
    std::uint16_t connect_timeout_;
    std::string application_name_;
  };

}
